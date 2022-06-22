async function logInProcedure(){
  let alertBox = document.getElementById("logInAlert");
  alertBox.style.display = "none";
  alertBox.innerHTML = "";

  let name = logInForm.name.value;
  let password = logInForm.password.value;

  if(!name){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Not a valid username";
    return;
  }else if(!password){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Empty password";
    return;
  }

  let data = {name:name, password:password};
  let token = await getJWT(data);

  if(!token){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Wrong username/password";
    logInForm.password.value = "";;
    return;
  }

  closeLogInModal();
  logInForm.name.value = "";;
  logInForm.password.value = "";;

  window.sessionStorage.setItem("accessToken", token.accessToken);
  pageLogIn(token.accessToken);
}

async function signUpProcedure(){
  let alertBox = document.getElementById("signUpAlert");
  alertBox.style.display = "none";
  alertBox.innerHTML = "";

  let name = signUpForm.name.value;
  let mail = signUpForm.mail.value;
  let password = signUpForm.password.value;
  let confirmPW = signUpForm.confirm.value;

  const reUname = /^[a-z0-9]+$/i;
  if(!name || name.lenght < 4 || !reUname.test(name)){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Username must be at least 4 alphanumerc characters";
    return;
  }else if(!mail || !validateEmailStructure(mail)){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Not a valid e-mail";
    return;
  }else if(!password){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Empty password";
    return;
  }else if(!confirmPW){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Confirm the password";
    return;
  }else if(password !== confirmPW){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Passwords don't match";
    return;
  }

  if(await !validateUsername(name)){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Username already in use";
    return;
  }else if(await !validateEmail(mail)){
    alertBox.style.display = "block";
    alertBox.innerHTML = "Email already in use";
    return;
  }

  let data = {name:name, password:password, email:mail};
  let created = await createUser(data);

  if(!created){
    alertBox.style.display = "block";
    alertBox.innerHTML = "The account was NOT created due to an error!";
    return;
  }

  closeSignUpModal();
  signUpForm.name.value = "";
  signUpForm.mail.value = "";
  signUpForm.password.value = "";
  signUpForm.confirm.value = "";

  var snack = document.getElementById("snackbar");
  snack.innerHTML = "Account created successfully." +
    "Log in to access the account";
  snack.className = "show";
  setTimeout(function(){ snack.className = snack.className.replace("show","");},
    3000);
}

function logOutProcedure(){
  window.sessionStorage.removeItem("accessToken");
  pageLogOut();
}

async function getJWT(data){
  let response = await fetch("api/login/", 
    {method: "POST", body: JSON.stringify(data), 
      headers:{"Content-Type":"application/json"}});

  if(response.status != 200){
    return "";
  }

  return await response.json();
}

async function createUser(data){
  let response = await fetch("api/users/", 
    {method: "POST", body: JSON.stringify(data), 
      headers:{"Content-Type":"application/json"}});

  return response.status == 201;
}

function validateEmailStructure(email) {
    const re = /[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?/;
    return re.test(String(email).toLowerCase());
}

async function validateUsername(name){
  let response = await fetch("api/users/?user=" + name);

  if(response.status != 200){
    return false;
  }

  let data = await response.json();
  return !data.exist;
}

async function validateEmail(email){
  let response = await fetch("api/users/?email=" + email);

  if(response.status != 200){
    return false;
  }

  let data = await response.json();
  return !data.exist;
}
