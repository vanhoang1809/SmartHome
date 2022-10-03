function resetPassword() {
    var userEmail = document.getElementById("email_field").value;
    var auth = firebase.auth();
    if (userEmail != "") {
        auth.sendPasswordResetEmail(userEmail).then(function() {
            window.alert("Email đã được gửi đến bạn, vui lòng kiểm tra Email");
        }).catch(function(error) {
            var errorCode = error.code;
            var errormessage = error.message;
            console.log(errorCode);
            console.log(errormessage);
            window.alert("Message: " + errormessage);
        });
    } else {
        window.alert("Vui lòng nhập email!");
    }
}