const arrayHasIndex = (array, index) => 
  Array.isArray(array) && array.hasOwnProperty(index);

const lines = 10;
var historyPage = 0;
var numMeasurements = 0;
var columns = [];
var indexes = [];

var updateTableInterval;

function generateTable(data){
  var newTable = document.createElement("table"),
    tBody = newTable.createTBody(),
    tHead = newTable.createTHead(),
    wrap = document.getElementById("historyTable");

  newTable.className = "historyTable";

  var row = generateRow(data, 0);
  tHead.appendChild(row);

  var nrows = (data!==null && data.length!==0) ? data.length+1 : 2;
  for(var i=1; i<nrows; i++) {
    row = generateRow(data, i);
    tBody.appendChild(row);
  }

  (wrap.hasChildNodes()? wrap.replaceChild : wrap.appendChild).call(wrap,
    newTable, wrap.firstElementChild);
}

function generateRow(data, line) {
  var row = document.createElement("tr");
  if(line==0){
    for(let i=0; i<columns.length; i++)
    {
      let cell = document.createElement("th");

      let t = columns[i];
      let text = document.createTextNode(t);

      cell.innerHTML = t;
      row.appendChild(cell);
    }

    return row;
  }

  if(data == null || data.length == 0){
    var text = document.createTextNode("No recorded data.");
    var cell = row.insertCell();
    cell.appendChild(text);
    cell.setAttribute("colspan",columns.length);
    cell.style.textAlign = "center";
    return row;
  }

  var info = data[line-1];
  var time = new Date(info.time);
  var t = [];
  time = time.toISOString().replace("T", " ").replace("Z", "");
  t[0] = time;
  for(let i=0; i<info.sensors.length; i++)
  {
    t[indexes[info.sensors[i]]] = info.values[i];
  }

  for(let i=0; i<columns.length; i++) {
    if(arrayHasIndex(t,i)) {
      var text = document.createTextNode(t[i]);
    }else{
      var text = document.createTextNode("~");
    }
    row.insertCell().appendChild(text);
  }

  return row;
}

async function selectHistoryVehicle(vehicle){
  selectedVehicle = vehicle;

  clearInterval(updateTableInterval);
  updateTableInterval = 0;
  if(!vehicle){
    let table = document.getElementById("historyTable");
    if(table.firstChild){
      table.removeChild(table.firstChild);
    }

    document.getElementById("historyTitle").innerHTML = "";
    deletePagination();

    document.getElementById("historyControls").style.display = "none";

    return;
  }

  document.getElementById("historyTitle").innerHTML = vehicle.name;
  let sensors = await getSensors(vehicle.id);
  document.getElementById("historyControls").style.display = "flex";

  columns = [];
  indexes = [];
  columns[0] = 'Time';

  let i = 1;
  for(const sensor of sensors){
    columns[i] = sensor.name;
    indexes[sensor.id] = i;
    i++;
  }

  await updateTable(vehicle.id);
  updateTableInterval = setInterval(function(){
    updateTable(vehicle.id)}, 10000);
}

async function updateTable(vehicleId){
  let data = await getMeasurements(selectedVehicle.id);
  generateTable(data);

  deletePagination();
  numMeasurements = await getNumberOfMeasurements(selectedVehicle.id);
  generatePagination();
}

async function populateHistoryVehicleList(){
  let vehicleList = document.getElementById("historyVehicles");
  vehicleList.innerHTML = "";

  let vehicles = await getVehicles();
  for(const vehicle of vehicles){
    let button = document.createElement("button");
    button.innerHTML = vehicle.name;
    button.onclick = function(){ historyPage = 0; 
      selectHistoryVehicle(vehicle); };

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

function generatePagination(){
  let div = document.getElementById("paginationControls");
  let pages = Math.floor(numMeasurements / lines);

  if(pages < 2){
    return;
  }

  let limit = pages<5?pages:5;
  let offset = historyPage>pages-3?4+historyPage-pages:2;
  let t = ["<<", "<"];
  let refs = [0, historyPage-1];
  for(let i=0, p=historyPage-offset; i<limit; i++, p++){
    while(p<0) p++;
    t.push((p+1).toString());
    refs.push(p);
  }
  t.push(">", ">>");
  refs.push(historyPage+1, pages);

  for(let i=0; i<t.length; i++){
    var node = document.createTextNode(t[i]);
    var link = document.createElement("a");
    link.setAttribute("href", "javascript:movePagination("+refs[i]+");");
    if(i>1 && refs[i]==historyPage && i<t.length-1){
      link.id="selectedPage";
    }
    link.appendChild(node);
    div.appendChild(link);
  }
}

function deletePagination(){
  let div = document.getElementById("paginationControls");
  while(div.firstChild){
    div.removeChild(div.firstChild);
  }
}

function movePagination(page){
  if(page<0 || page>Math.floor(numMeasurements/lines)) return;
  historyPage = page;

  deletePagination();
  generatePagination();
  updateTable(selectedVehicle.id);
}

async function clearHistory(){
  if(confirm("Do you wish to delete all stored data?")) {
    let res = await deleteMeasurements(selectedVehicle.id);
    if(res){
      snackbar("Data sucessfully deleted");
      updateTable(selectedVehicle.id);
      deletePagination();
      generatePagination();
    }
  }
}

async function download(vehicleId){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + vehicleId + "/measurements/";

  let response = await fetch(url, {headers:{"Authorization": auth}});
  if(!response.ok || response.length==0){
    return undefined;
  }

  let data = await response.json();
  var csv = "data:text/csv;charset=utf-8,";
  for(let i=0; i<columns.length; i++){
    csv += columns[i];
    if(i != columns.length - 1){
      csv += ",";
    }
  }
  csv += "\r\n";

  data.forEach(function (info){
    let time = new Date(info.time);
    let t = [];
    time = time.toISOString().replace("T", " ").replace("Z", "");
    t[0] = time;

    for(let i=0; i<info.sensors.length; i++) {
      t[indexes[info.sensors[i]]] = info.values[i];
    }

    for(let j=0; j<columns.length; j++){
      if(t[j]){
        csv += t[j];
      }
      if(j != columns.length - 1){
        csv += ",";
      }
    }

    csv += "\r\n";
  });

  time = new Date(data[0].time);
  time = time.toISOString().replace("T", "_").replace("Z", "")
    .replaceAll("-","_").replaceAll(":","_").replace(".","_");
  name = time + ".csv";

  var encodedURI = encodeURI(csv);
  var link = document.createElement("a");
  link.setAttribute("href", encodedURI);
  link.setAttribute("download", time+".csv");
  document.body.appendChild(link);
  link.click();
}

async function getMeasurements(vehicleId){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + vehicleId + "/measurements";

  url += "?limit=" + lines + "&offset=" + historyPage * lines;

  let response = await fetch(url, {headers:{"Authorization": auth}});

  if(!response.ok){
    return undefined;
  }

  return await response.json();
}

async function getNumberOfMeasurements(vehicleId){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + vehicleId + "/measurements?count=true";

  let response = await fetch(url, {headers:{"Authorization": auth}});

  if(!response.ok){
    return undefined;
  }

  let info = await response.json();
  return info.count;
}

async function deleteMeasurements(vehicleId){
  let auth = "Bearer " + window.sessionStorage.getItem("accessToken");
  let url = "api/vehicles/" + vehicleId + "/measurements/";

  let response = await fetch(url, {method:"DELETE",
    headers:{"Authorization": auth}});

  return response == 204;
}
