var selectedVehicle;

async function createVehicleProcedure(){
  let alertBox = document.getElementById("createVehicleAlert");
  alertBox.style.display = "none";
  alertBox.innerHTML = "";

  let name = createVehicleForm.vName.value;
  let color = createVehicleForm.vColor.jscolor.toString();

  const reName = /^[a-z0-9]+$/i;
  if(!name || name.length < 2 || !reName.test(name)){
    alertBox.style.display = "block";
    alertBox.innerHTML = "The vehicle name must be at least 2 alphanumerc " +
      "characters";
    return;
  }else if(!color){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Select a color for the new vehicle";
    return;
  }

  let data = {name:name, color:color};
  let created = await createVehicle(data);

  if(!created){
    alertBox.style.display = "block";
    alertBox.innerHTML = "This vehicle was not created due to an error!";
    return;
  }

  closeCreateVehicleModal();
  createVehicleForm.vName.value = "";
  createVehicleForm.vColor.value = "#3399FF";

  snackbar("Vehicle created successfully");

  updateVehicleList();
}

async function editVehicleProcedure(){
  var snack = document.getElementById("snackbar");
  let name = editVehicleForm.vName.value;
  let color = editVehicleForm.vColor.jscolor.toString();

  const reName = /^[a-z0-9]+$/i;
  if(!name || name.length < 2 || !reName.test(name)){
    snackbar("Vehicle not edited: Invalid name");
    return;
  }else if(!color){
    snackbar("Vehicle not edited: Invalid color");
    return;
  }

  let data = {name:name, color:color};
  let edited = await editVehicle(data);

  if(!edited){
    snackbar("Vehicle not edited! Server error");
    return;
  }

  snackbar(selectedVehicle.name + " edited sucessfully.");

  updateVehicleList();
}

async function deleteVehicleProcedure(){
  let result = confirm("Delete " + selectedVehicle.name + "?");

  if(!result){
    return;
  }

  let deleted = await deleteVehicle();

  var snack = document.getElementById("snackbar");
  if(deleted){
    snackbar(selectedVehicle.name + " deleted");
  }else{
    snackbar("An error occurred, the vehicle was not deleted.");
    return;
  }

  selectVehicle(undefined);
}

async function sensorEditProcedure(form){
  let id = form.firstChild.firstChild.data.substring(1);
  let name = form.sensorName.value;

  let type;
  for(const radio of form.type){
    if(radio.checked){
      type = radio.value;
      break;
    }
  }

  if(name.length < 2){
    snackbar("Sensor not edited: Invalid name");
    return;
  }else if(type == undefined){
    snackbar("Sensor not edited: Sensor type not selected");
    return;
  }

  data = {name : name, data_type : type};
  let edited = await editSensor(id, data);

  if(!edited){
    snackbar("Sensor not edited! Server error");
    return;
  }

  snackbar("Sensor #" + id + " edited sucessfully.");
}

async function sensorDeleteProcedure(button){
  let form = button.parentElement;
  let id = form.firstChild.firstChild.data.substring(1);

  let deleted = await deleteSensor(id);

  if(!deleted){
    snackbar("Sensor not deleted! Server error");
    return;
  }

  form.parentElement.removeChild(form);
  snackbar("Sensor successfully deleted");
}

async function updateSensorList(){
  let list = document.getElementById("sensorList");

  let c;
  while(c = list.firstChild){
    list.removeChild(c);
  }

  if(selectedVehicle){
    let sensors = await getSensors(selectedVehicle.id);

    for(const sensor of sensors){
      let data = {id: sensor.id, name: sensor.name, type: sensor.data_type};
      makeSensorForm(data);
    }
  }
}

function selectVehicle(vehicle){
  selectedVehicle = vehicle;

  if(vehicle){
    document.getElementById("vehicleOptions").style.display = "";
    document.getElementById("vehicleSensors").style.display = "";

    editVehicleForm.vName.value = vehicle.name;
    editVehicleForm.vName.style.background = vehicle.color;
    editVehicleForm.vName.style.color = getContrastYIQ(vehicle.color);
    editVehicleForm.vColor.jscolor.fromString(vehicle.color);
    document.getElementById("vehicleIdLabel").innerHTML = "#" + vehicle.id;
  }else{
    document.getElementById("vehicleOptions").style.display = "none";
    document.getElementById("vehicleSensors").style.display = "none";
  }

  updateVehicleList();
  updateSensorList();
}

async function updateVehicleList(){
  let vehicleList = document.getElementById("vehicles");
  vehicleList.innerHTML = "";

  let vehicles = await getVehicles();
  for(const vehicle of vehicles){
    let button = document.createElement("button");
    button.innerHTML = vehicle.name;
    button.onclick = function(){ selectVehicle(vehicle); };

    button.style.backgroundColor = vehicle.color;
    button.style.color = getContrastYIQ(vehicle.color);

    if(!selectedVehicle || selectedVehicle.id != vehicle.id){
      button.id = "";
    }else{
      button.id = "selectedVehicle";
    }

    vehicleList.appendChild(button);
  }
}

async function addSensor(){
  let number = await createSensor({name: "Sensor", data_type: "float"});

  if(!number){
    snackbar("An error occurred, could not create the sensor.");
    return;
  }

  makeSensorForm({id: number, name: "Sensor", type: "float"});
}

function makeSensorForm(data){
  let form = document.createElement("form");
  form.className = "sensorForm";

  let id = document.createElement("label");
  id.setAttribute("for", "sensorName");
  id.setAttribute("name", "sensorNameLabel");
  id.className = "idLabel";
  id.innerHTML = "#" + data.id;

  let name = document.createElement("input");
  name.setAttribute("type", "text");
  name.setAttribute("name", "sensorName");
  name.value = data.name;

  let radioFloat = document.createElement("input");
  radioFloat.setAttribute("type", "radio");
  radioFloat.setAttribute("name", "type");
  radioFloat.value = "float";
  let labelFloat = document.createElement("label");
  labelFloat.innerHTML = "float";
  let divFloat = document.createElement("div");
  divFloat.append(radioFloat, labelFloat);

  let radioInt = document.createElement("input");
  radioInt.setAttribute("type", "radio");
  radioInt.setAttribute("name", "type");
  radioInt.value = "int";
  let labelInt = document.createElement("label");
  labelInt.innerHTML = "int";
  let divInt = document.createElement("div");
  divInt.append(radioInt, labelInt);

  let radioString = document.createElement("input");
  radioString.setAttribute("type", "radio");
  radioString.setAttribute("name", "type");
  radioString.value = "string";
  let labelString = document.createElement("label");
  labelString.innerHTML = "string";
  let divString = document.createElement("div");
  divString.append(radioString, labelString);

  if(data.type == "float"){
    radioFloat.checked = true;
  }else if(data.type == "int"){
    radioInt.checked = true;
  }else if(data.type == "string"){
    radioString.checked = true;
  }

  let radioDiv = document.createElement("div");
  radioDiv.className = "radioDiv";
  radioDiv.append(divFloat, divInt, divString);

  let edit = document.createElement("button");
  edit.innerHTML = "Edit";
  edit.setAttribute("type", "submit");
  edit.value = "edit";

  let del = document.createElement("button");
  del.innerHTML = "Delete";
  /*del.setAttribute("type", "button");*/
  del.value = "delete";
  del.setAttribute("onClick", "sensorDeleteProcedure(this);" +
    "return false;");

  form.append(id, name, radioDiv, edit, del);
  form.setAttribute("onSubmit", "sensorEditProcedure(this); return false;");

  let list = document.getElementById("sensorList");
  list.appendChild(form);
}

async function getVehicles(){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/users/" + userID + "/vehicles/";
  let response = await fetch(url, {headers:{"Authorization": auth}});

  return await response.json();
}

async function createVehicle(data){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/users/" + userID + "/vehicles/";
  let response = await fetch(url,
    {method: "POST", body: JSON.stringify(data),
      headers:{"Content-Type":"application/json", 
      "Authorization": auth}});

  if(!response.ok){
    return 0;
  }

  let loc = response.headers.get("Content-Location").split('/');
  let num = loc[loc.length-1];
  return parseInt(num);
}

async function editVehicle(data){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + selectedVehicle.id + "/";
  let response = await fetch(url,
    {method: "PUT", body: JSON.stringify(data),
      headers:{"Content-Type":"application/json", 
      "Authorization": auth}});

  return response.status == 204;
}

async function deleteVehicle(){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + selectedVehicle.id + "/";
  let response = await fetch(url,
    {method: "DELETE", headers: {"Authorization": auth}});

  return response.status == 204;
}

async function getSensors(id){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + id + "/sensors/";
  let response = await fetch(url, {headers:{"Authorization": auth}});

  return await response.json();
}

async function createSensor(data){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + selectedVehicle.id + "/sensors/";
  let response = await fetch(url,
    {method: "POST", body: JSON.stringify(data),
      headers:{"Content-Type":"application/json", 
      "Authorization": auth}});

  if(!response.ok){
    return 0;
  }

  let loc = response.headers.get("Content-Location").split('/');
  let num = loc[loc.length-1];
  return parseInt(num);
}

async function editSensor(id, data){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + selectedVehicle.id + "/sensors/" + id + "/";
  let response = await fetch(url,
    {method: "PUT", body: JSON.stringify(data),
      headers:{"Content-Type":"application/json", 
      "Authorization": auth}});

  return response.status == 204;
}

async function deleteSensor(id){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + selectedVehicle.id + "/sensors/" + id + "/";
  let response = await fetch(url,
    {method: "DELETE", headers:{"Authorization": auth}});

  return response.status == 204;
}

function changePreviewColor(picker){
  let color = editVehicleForm.vColor.jscolor.toString();
  editVehicleForm.vName.style.backgroundColor = color;
  editVehicleForm.vName.style.color = getContrastYIQ(color);
}

/* https://stackoverflow.com/questions/11867545 */
function getContrastYIQ(hexcolor){
    hexcolor = hexcolor.replace("#", "");
    var r = parseInt(hexcolor.substr(0,2),16);
    var g = parseInt(hexcolor.substr(2,2),16);
    var b = parseInt(hexcolor.substr(4,2),16);
    var yiq = ((r*299)+(g*587)+(b*114))/1000;
    return (yiq >= 128) ? 'black' : 'white';
}

