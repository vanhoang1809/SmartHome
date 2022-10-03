function login() {
    var userEmail = document.getElementById("email_field").value;
    var userPassword = document.getElementById("password_field").value;
    if (userEmail != "" && userPassword != "") {
        firebase.auth().signInWithEmailAndPassword(userEmail, userPassword)
            .then(function() {
                window.location = "../View/index.html";

                // ...
            })
            .catch((error) => {

                var errorCode = error.code;
                var errormessage = error.message;
                console.log(errorCode);
                console.log(errormessage);
                window.alert("Message: " + errormessage);
            });
    } else {
        window.alert("Bạn chưa nhập mật khẩu hoặc Email!");
    }
}