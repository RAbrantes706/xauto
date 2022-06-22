var userID;
var username;

var selectedFrame;

init();

function init(){
  let token = window.sessionStorage.getItem("accessToken");
  if(token){
    pageLogIn(token);
  }

  selectedFrame = window.sessionStorage.getItem("selectedFrame");
  if(!selectedFrame){
    selectedFrame = "Home";
  }else{
    selectFrame(selectedFrame);
  }
}

function pageLogIn(accessToken){
  let payload = parseJwt(accessToken);
  userID = payload.user_id;
  username = payload.user_name;

  /* Take care of the nav */
  document.getElementById("navLogIn").style.display = "none";
  document.getElementById("navSignUp").style.display = "none";

  document.getElementById("navUser").innerHTML = username;
  document.getElementById("navUser").style.display = "";
  document.getElementById("navLogOut").style.display = "";

  document.getElementById("navDashboard").style.display = "";
  document.getElementById("navVehicles").style.display = "";
  document.getElementById("navHistory").style.display = "";
}

function pageLogOut(){
  /* Take care of the nav */
  document.getElementById("navLogIn").style.display = "";
  document.getElementById("navSignUp").style.display = "";

  document.getElementById("navUser").innerHTML = "";
  document.getElementById("navUser").style.display = "none";
  document.getElementById("navLogOut").style.display = "none";

  document.getElementById("navDashboard").style.display = "none";
  document.getElementById("navVehicles").style.display = "none";
  document.getElementById("navHistory").style.display = "none";

  selectFrame("Home");
}

function selectFrame(frame){
  if(selectedFrame == "Dashboard"){
    clearInterval(updateDashboardInterval);
    updateDashboardInterval = 0;
    clearDashboard();
  }else if(selectedFrame == "History"){
    selectHistoryVehicle(undefined);
  }

  if(frame == "Vehicles"){
    selectVehicle(undefined);
  }else if(frame == "History"){
    selectHistoryVehicle(undefined);
    populateHistoryVehicleList();
  }else if(frame == "Dashboard"){
    buildDashboard();
  }

  let oldFrame = document.getElementById("frame" + selectedFrame);
  let selectedFrameAnchor = document.getElementById("nav" + selectedFrame);
  oldFrame.style.display = "none";
  selectedFrameAnchor.classList.remove("selected");
  
  let newFrame = document.getElementById("frame" + frame);
  let newSelectedFrameAnchor = document.getElementById("nav" + frame);
  newFrame.style.display = "";
  newSelectedFrameAnchor.classList.add("selected");

  selectedFrame = frame;
  window.sessionStorage.setItem("selectedFrame", frame);
}

function snackbar(text){
  var snack = document.getElementById("snackbar");
  snack.innerHTML = text;
  snack.className = "show";
  setTimeout(
    function(){ snack.className = snack.className.replace("show",""); },
    3000);
}

/* https://stackoverflow.com/questions/38552003 */
function parseJwt (token) {
    var base64Url = token.split('.')[1];
    var base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/');
    var jsonPayload = decodeURIComponent(atob(base64).split('').map(function(c) {
        return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
    }).join(''));

    return JSON.parse(jsonPayload);
};
