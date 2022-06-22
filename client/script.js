const arrayHasIndex = (array, index) => 
  Array.isArray(array) && array.hasOwnProperty(index);
const lines = 60;

var selectedVehicle;

var numMeasurements;
var activePage = 0;
var columns = [];
var indexes = [];

var more = false;

var vehicles = [];
var owners = [];

init();

setInterval(updateValues, 1000);

function down(){
  let url = "api/vehicles/" + selectedVehicle + "/measurements";
  fetch(url).then(
    function (response){
      if(response.status !== 200){
        console.log('Problem: ' + response.status);
        return;
      }
      if(response.length==0) return;

      response.json().then((data)=>{
        var csv = "data:text/csv;charset=utf-8,";
        columns.forEach(function(column){
          csv += column + ",";
        });
        csv += "\r\n";

        data.forEach(function (info){
          var time = new Date(info.time);
          var t = [];
          time = time.toISOString().replace("T", " ").replace("Z", "");
          t[0] = time;

          for(let i=0; i<info.sensors.length; i++) {
            t[indexes[info.sensors[i]]] = info.values[i];
          }

          t.forEach(function (data){
            csv += data + ",";
          });
          csv += "\r\n";
        });

        var time = new Date(data[0].time);
        time = time.toISOString().replace("T", "_").replace("Z", "")
          .replaceAll("-","_").replaceAll(":","_").replace(".","_");
        name = time + ".csv";

        var encodedURI = encodeURI(csv);
        var link = document.createElement("a");
        link.setAttribute("href", encodedURI);
        link.setAttribute("download", time+".csv");
        document.body.appendChild(link);
        link.click();
      });
    }).catch((error)=>console.log(error));
}

function getSensors(){
  let i = 1;
  let url = "api/vehicles/" + selectedVehicle + "/sensors";
  fetch(url)
    .then(
      function (response){
        if(response.status !== 200) {
          console.log('Problem: ' + response.status);
          return;
        }
        if(response.length==0) return;

        columns = [];
        indexes = [];
        columns[0] = 'Time';

        response.json().then(function (data) {
          data.forEach(function (column)
            {
              columns[i] = column.name;
              indexes[column.id] = i;
              i++;
            });
          });
      }).catch(function (error){
        console.log('Fetch error : -S', error);
      });
}

function generateTable(data){
  var newTable = document.createElement("table"),
    tBody = newTable.createTBody(),
    wrap = document.getElementById("wrap");

  var pag = document.getElementById("pagination-controls");
  if(more){
    if(!pag.firstChild) generateControls(pag);
  }else{
    deleteControls(pag);
    activePage = 0;
  }

  var nrows = (more && data!==null && data.length!==0) ? data.length+1 : 2;
  for(var i=0; i<nrows; i++) {
    var row = generateRow(data, i);
    tBody.appendChild(row.cloneNode(true));
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

      row.insertCell().appendChild(cell);
      cell.innerHTML = t;
    }
    return row;
  }
  if(data == null || data.length == 0){
    var text = document.createTextNode("No recorded data.");
    row.insertCell().appendChild(text);
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

function generateControls(div){
  let pages = Math.floor(numMeasurements / lines);

  let limit = pages<5?pages:5;
  let offset = activePage>pages-3?4+activePage-pages:2;
  t = ["<--", "<"];
  refs = [0, activePage-1];
  for(let i=0, p=activePage-offset; i<limit; i++, p++){
    while(p<0) p++;
    t.push((p+1).toString());
    refs.push(p);
  }
  t.push(">", "-->");
  refs.push(activePage+1, pages);

  for(let i=0; i<t.length; i++){
    var node = document.createTextNode(t[i]);
    var link = document.createElement("a");
    link.setAttribute("href", "javascript:move("+refs[i]+");");
    if(i>1 && refs[i]==activePage && i<t.length-1){
      link.id="selected-page";
    }
    link.appendChild(node);
    div.appendChild(link);
  }
}

function deleteControls(div){
  while(div.firstChild){
    div.removeChild(div.firstChild);
  }
}

function move(page){
  if(page<0 || page>Math.floor(numMeasurements/lines)) return;
  activePage = page;

  var pag = document.getElementById("pagination-controls");
  deleteControls(pag);
  generateControls(pag);
  updateValues();

}

function toogleMore(){
  more = !more;
  document.getElementById("more").innerHTML = more ? "Less..." : "More...";
  updateValues();
}

function compare(a, b){
  if(a.id < b.id) return -1;
  if(a.id > b.id) return 1;
  return 0;
}

function updateValues(){
  let url = "api/vehicles/" + selectedVehicle + "/measurements";
  if(!more) {
    url += "?limit=1";
  }else{
    fetch(url + "?count=true").
      then(response => {
        response.json().then( info => numMeasurements = info.count );
      }).catch( error => console.log(error) );

    url += "?limit=" + lines + "&offset=" + activePage * lines;
  }

  fetch(url)
    .then(
      function (response) {
        if(response.status !== 200) {
          console.log('Problem: ' + response.status);
          return;
        }
        if(response.length==0) return;

        response.json().then(function (data) {
          generateTable(data);
        });
      }).catch(function (error) {
        console.log(error);
      });
}

function deleteData() {
  if(confirm("Do you wish to delete all stored data?")) {
    fetch('./api/vehicles/3/measurements/', {method:'DELETE'})
    generateTable(null);
  }
}

function selectVehicle(id){
  selectedVehicle = id;
  getSensors();
  updateValues();

  let vehicleName = vehicles[id];
  let title = document.getElementById("selected-vehicle");
  let text = document.createTextNode(vehicleName);

  document.title = vehicleName;
  title.hasChildNodes() ? title.replaceChild(text, title.firstChild) : 
    title.appendChild(text);
}

async function getVehicles(){
  let url = "api/vehicles";

  let response = await fetch(url);

  if(response.status !== 200) {
    console.log('Problem: ' + response.status);
    return;
  }
  if(response.length==0) return;

  let data = await response.json();
  for(let i=0; i<data.length; i++){
    vehicle = data[i];

    vehicles[vehicle.id] = vehicle.name;
    let linkText = "";

    if(!arrayHasIndex(owners, vehicle.owner)){
      let userResponse = await fetch("api/users/" + vehicle.owner);
      let user = await userResponse.json();
      owners[user.id] = user.name;
    }
    linkText = owners[vehicle.owner];
    linkText += "-" + vehicle.name;

    let text = document.createTextNode(linkText);
    let link = document.createElement("a");
    link.href = "javascript:selectVehicle("+vehicle.id+");";
    link.appendChild(text);
    document.getElementById("vehicles-list").appendChild(link);
  }
}

async function init(){
  await getVehicles();
  selectVehicle(3);
}
