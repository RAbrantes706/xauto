window.onclick = function(event){
  let logInModal = document.getElementById("logInModal");
  let signUpModal = document.getElementById("signUpModal");
  if(event.target == logInModal){
    closeLogInModal();
  }else if(event.target == signUpModal){
    closeSignUpModal();
  }
}

function openLogInModal(){
  let modal = document.getElementById("logInModal");
  modal.style.display="block";
}

function closeLogInModal(){
  let modal = document.getElementById("logInModal");
  let alertBox = document.getElementById("logInAlert");
  modal.style.display="none";
  alertBox.style.display = "none";
  alertBox.innerHTML = "";
}

function openSignUpModal(){
  let modal = document.getElementById("signUpModal");
  modal.style.display="block";
}

function closeSignUpModal(){
  let modal = document.getElementById("signUpModal");
  let alertBox = document.getElementById("signUpAlert");
  modal.style.display="none";
  alertBox.style.display = "none";
  alertBox.innerHTML = "";
}

function openCreateVehicleModal(){
  let modal = document.getElementById("createVehicleModal");
  createVehicleForm.vColor.jscolor.randomize();
  modal.style.display="block";
}

function closeCreateVehicleModal(){
  let modal = document.getElementById("createVehicleModal");
  let alertBox = document.getElementById("createVehicleAlert");
  modal.style.display="none";
  alertBox.style.display = "none";
  alertBox.innerHTML = "";
}
