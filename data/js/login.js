document.addEventListener('DOMContentLoaded',()=>{
    
    const loginForm=document.getElementById('login-form');
    const userInp = document.getElementById('username');
    const passwordInp = document.getElementById('password');
    const userError = document.getElementById("user-error");
    const passwordError = document.getElementById("password-error");

    const eyeWrapper = document.getElementById("eye-wrapper");
    const eyeOpen = document.getElementById("eye-open");
    const eyeClosed = document.getElementById("eye-closed");

    if(eyeWrapper){
            eyeWrapper.addEventListener("click",()=>{
                const isPassword = passwordInp.type === "password";
                passwordInp.type = isPassword ? "text" : "password";

                if(isPassword){
                    eyeOpen.style.display = "block";
                    eyeClosed.style.display = "none"
                }
                else{
                    eyeOpen.style.display = "none";
                    eyeClosed.style.display = "block";
                }
            })
        }

    loginForm.addEventListener('submit', (e)=>{
        e.preventDefault();

    const userVal = document.getElementById('username').value
    const passwordVal = document.getElementById('password').value

       

        userError.style.display = "none"
        passwordError.style.display = "none"
        userInp.classList.remove("input-error")
        passwordInp.classList.remove("input-error")

        let hasError = false
        
        if(userVal.length === 0){
            userError.innerText = "This field is required"
            userError.style.display = "block"
            userInp.classList.add("input-error")
            hasError = true
        }
        else if(userVal.length <= 3){
            userError.innerText = "The Id must longer than 3 letters"
            userError.style.display = "block"
            userInp.classList.add("input-error")
            hasError = true;
        }
       
        if(passwordVal.length === 0){
            passwordError.innerText = "This field is required"
            passwordError.style.display = "block"
            passwordInp.classList.add("input-error")
            hasError = true
        }
        else if(passwordVal.length <= 3){
            passwordError.innerText = "The Password must longer than 3 letters"
            passwordError.style.display = "block"
            passwordInp.classList.add("input-error")
            hasError = true;
        }


        if(!hasError){
            if(userVal === "admin" && passwordVal === "1234"){
                window.location.href = "/data/dashboard.html";
            }
            else{
                passwordError.innerText = "ID or Password incorect"
                passwordError.style.display = "block"
            }
        }
    });
});