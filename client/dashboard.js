var dashboard;

var updateDashboardInterval;

async function buildDashboard(){
  clearDashboard();
  let dashboardDiv = document.getElementById("dashboard");

  let vehicles = await getVehicles();
  for(const vehicle of vehicles){
    let vehicleDiv = document.createElement("div");
    let sensorsDiv = document.createElement("div");
    let vehicleName = document.createElement("h2");

    vehicleDiv.className = "vehicleDashboard";
    sensorsDiv.className = "sensorsDashboard";
    vehicleName.innerHTML = vehicle.name;
    vehicleDiv.appendChild(vehicleName);
    vehicleDiv.appendChild(sensorsDiv);

    let object = { vehicle : vehicle, div : vehicleDiv, sensors : [], 
      circle : undefined };

    let sensors = await getSensors(vehicle.id);
    let measurements = await getCurrentMeasurements(vehicle.id);

    let lat = false;
    let lon = false;
    for(const sensor of sensors){
      let sensorDiv = document.createElement("div");
      let sensorName = document.createElement("div");
      let sensorValue = document.createElement("div");

      let value;
      if(measurements){
        let index = measurements.sensors.findIndex(e => e == sensor.id);
        value = index < 0 ? "~" : measurements.values[index];
      }else{
        value = "~";
      }

      sensorDiv.className = "sensorBox";
      sensorDiv.style.backgroundColor = vehicle.color;
      sensorDiv.style.color = getContrastYIQ(vehicle.color);
      sensorName.className = "sensorBoxName";
      sensorValue.className = "sensorBoxValue";
      sensorName.innerHTML = sensor.name;
      sensorValue.innerHTML = value;
      sensorDiv.appendChild(sensorName);
      sensorDiv.appendChild(sensorValue);

      sensorsDiv.appendChild(sensorDiv);

      if(sensor.name.toLowerCase() == "latitude"){
        lat = true;
      }else if(sensor.name.toLowerCase() == "longitude"){
        lon = true;
      }

      let sensorObj = { sensor : sensor, div : sensorDiv, nameDiv : sensorName,
        valueDiv : sensorValue};
      object.sensors.push(sensorObj);
    }

    if(lat && lon){
      object.circle = L.circle([51.5, -0.11], { color : vehicle.color, 
        radius : 3}).addTo(dashboardMap);
    }

    dashboardDiv.appendChild(vehicleDiv);
    dashboard.push(object);
  }

  updateDashboardInterval = setInterval(updateDashboard, 1000);
}

async function updateDashboard(){
  for(const element of dashboard){
    let measurements = await getCurrentMeasurements(element.vehicle.id);
    let latitude, longitude;
    for(const board of element.sensors){
      let value;
      if(measurements){
        let sensor = board.sensor;
        let index = measurements.sensors.findIndex(e => e == sensor.id);
        value = index < 0 ? "~" : measurements.values[index];

        if(element.circle && index >= 0){
          if(sensor.name.toLowerCase() == "latitude"){
            latitude = value;
          }else if(sensor.name.toLowerCase() == "longitude"){
            longitude = value;
          }
        }
      }else{
        value = "~";
      }
      board.valueDiv.innerHTML = value;
    }
    if(latitude && longitude){
      element.circle.setLatLng({lat : latitude, lng : longitude});
    }
  }
  window.dispatchEvent(new Event('resize'));
}

function clearDashboard(){
  if(dashboard){
    for(const vehicle of dashboard){
      if(vehicle.circle){
        dashboardMap.removeLayer(vehicle.circle);
        vehicle.cirlce = undefined;
      }
    }
  }

  dashboard = [];
  let div = document.getElementById("dashboard");

  while(div.firstChild){
    div.removeChild(div.firstChild);
  }
}

async function getCurrentMeasurements(vehicleId){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + vehicleId + "/measurements?limit=1";

  let response = await fetch(url, {headers:{"Authorization": auth}});

  if(!response.ok){
    return undefined;
  }

  let info = await response.json();
  if(info.length){
    return info[0];
  }else{
    return undefined;
  }
}
