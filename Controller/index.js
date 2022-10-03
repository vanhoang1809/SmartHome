function makeid() {
  var text = "";
  //Chuoi so ngau nhien
  var possible =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  for (var i = 0; i < 5; i++)
    //gan gia tri cho text su dung math.floor de lam tron ham random
    text += possible.charAt(Math.floor(Math.random() * possible.length));
  return text;
}

//tao 1 phien may khach
var client = new Paho.MQTT.Client("broker.hivemq.com", 8000, makeid());
//Đặt các trình xử lý gọi lại
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

//Tạo đối tượng options
var options = {
  useSSL: false,
  userName: "",
  password: "",
  onSuccess: onConnect,
  onFailure: doFail,
};
var g = new JustGage({
  id: "gauge",
  value: 45,
  min: 0,
  max: 100,
});
var g2 = new JustGage({
  id: "gauge2",
  value: 45,
  min: 0,
  max: 50,
});
//Kết nối đến máy khách
client.connect(options);

function doFail(e) {
  console.log(e);
  document.getElementById("icon_connect").style.color = "red";
}

function onConnect() {
  // window.alert("Chào mừng bạn đến với HyHome");
  document.getElementById("icon_connect").style.color = "#00d084";
  document.getElementById("title_connect").innerHTML = "Đã Kết Nối";
  console.log("Kết nối thành công đến broker");
  client.subscribe("Esp32/sensors");
}

//Gọi hàm onConnectionLost khi bị mất kết nối
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.log(responseObject.errorMessage);
  }
}

//Gọi hàm khi có message đến
var status_led_living;
var status_fan_living;
var status_door_living;
var status_warning_sensors;
var status_gas_sensors;

function onMessageArrived(message) {
  console.log(message.destinationName + ":" + message.payloadString);
  var dataSever = message.payloadString;
  //Chuyển đối tượng JSON thành javascript
  var dataJson = JSON.parse(dataSever);

  var num_temperature = dataJson.temperature;
  var num_humidity = dataJson.humidity;
  //Làm tròn
  num_temperature = Math.round(num_temperature * 10) / 10;
  num_humidity = Math.round(num_humidity * 10) / 10;
  //  document.writeln(message.destinationName + ":" + message.payloadString);
  document.getElementById("message_sensors_temperature").innerHTML =
    num_temperature + "°C";
  document.getElementById("message_sensors_humidity").innerHTML =
    num_humidity + "%";
  var status_humi_chart = num_humidity;
  g.refresh(status_humi_chart);

  document.getElementById("note_chart_temp").innerHTML = num_temperature;
  document.getElementById("note_chart_humi").innerHTML = num_humidity;
  var status_temp_chart = num_temperature;
  g2.refresh(status_temp_chart);
  //Dùng cho chart DHT11

  //SENSOR WARNING
  status_warning_sensors = dataJson.infrared_sensor;
  if (status_warning_sensors == "1") {
    document.getElementById("user_warnng_ninja").style.display = "block";
    document.getElementById("user_warnng_lock").style.display = "none";
    document.getElementById("note_noPerson").style.display = "none";
    document.getElementById("note_Person").style.display = "block";
    document.getElementById("content_warning_sensor").style.background =
      "#ff6900";
  } else if (status_warning_sensors == "0") {
    document.getElementById("user_warnng_ninja").style.display = "none";
    document.getElementById("user_warnng_lock").style.display = "block";
    document.getElementById("note_noPerson").style.display = "block";
    document.getElementById("note_Person").style.display = "none";
    document.getElementById("content_warning_sensor").style.background =
      "#f7f4f1";
  }
  status_warning_sensors = dataJson.gas_sensor;
  if (status_warning_sensors == "1") {
    document.getElementById("gas_warnng_lock").style.display = "block";
    document.getElementById("gas_warnng_fire").style.display = "none";
    document.getElementById("note_gas").style.display = "none";
    document.getElementById("note_nogas").style.display = "block";
    document.getElementById("content_gas_sensor").style.background = "#f7f4f1";
    document.getElementById("content_gas_sensor").style.border =
      "5px solid rgb(0, 247, 255)";
  } else if (status_fan_living == "0") {
    document.getElementById("gas_warnng_lock").style.display = "none";
    document.getElementById("gas_warnng_fire").style.display = "block";
    document.getElementById("note_gas").style.display = "block";
    document.getElementById("note_nogas").style.display = "none";
    // document.getElementById("content_gas_sensor").style.background = "#d0021b";
    document.getElementById("content_gas_sensor").style.background = "#f73b3b";
    document.getElementById("content_gas_sensor").style.border =
      "5px solid #f73b3b";
  }
  ///CONTROL
  status_fan_living = dataJson.current_fanState_living;
  if (status_fan_living == "1") {
    document.getElementById("status_fan_living_on").style.display = "block";
    document.getElementById("status_fan_living_off").style.display = "none";
  } else if (status_fan_living == "0") {
    document.getElementById("status_fan_living_on").style.display = "none";
    document.getElementById("status_fan_living_off").style.display = "block";
  }
  status_led_living = dataJson.current_ledState_living;
  //document.getElementById("").innerHTML = status_led;
  if (status_led_living == "1") {
    document.getElementById("status_led_living_on").style.display = "block";
    document.getElementById("status_led_living_off").style.display = "none";
  } else if (status_led_living == "0") {
    document.getElementById("status_led_living_on").style.display = "none";
    document.getElementById("status_led_living_off").style.display = "block";
  }
  status_door_living = dataJson.current_servo_living;
  if (status_door_living == "1") {
    document.getElementById("status_door_living_on").style.display = "block";
    document.getElementById("status_door_living_off").style.display = "none";
  } else if (status_door_living == "0") {
    document.getElementById("status_door_living_on").style.display = "none";
    document.getElementById("status_door_living_off").style.display = "block";
  }

  /*********************************CHART_DHT11************************** */

  /*********************************************************** */
}

function button_led_living() {
  if (status_led_living == "0") {
    // post to firebase
    public("Esp32/control", "led1On");
    status_led_living == "1";
  } else {
    public("Esp32/control", "led1Off");
    status_led_living = "0";
  }
}

function button_fan_living() {
  if (status_fan_living == "0") {
    // post to firebase
    public("Esp32/control", "fan1On");
    status_fan_living == "1";
  } else {
    public("Esp32/control", "fan1Off");
    status_fan_living = "0";
  }
}

function button_door_living() {
  if (status_door_living == "0") {
    // post to firebase
    public("Esp32/control", "servo1On");
    status_door_living == "1";
  } else {
    public("Esp32/control", "servo1Off");
    status_door_living = "0";
  }
}

//Xuất bản 1 tin nhắn tới 1 topic nào đó
function public(topic, data) {
  message = new Paho.MQTT.Message(data);
  message.destinationName = topic;
  client.send(message);
}

//Open Weather API
link =
  "https://api.openweathermap.org/data/2.5/weather?q=Hoi An,VN&appid=bf8d15a80c89aa4f4c82ad6cbb3f5ac5";
var request = new XMLHttpRequest();
request.open("GET", link, true);
request.onload = function () {
  var obj = JSON.parse(this.response);
  var num_temperature;
  var num_feesLike;
  var num_visibility;
  console.log(obj);

  //document.getElementById('weather').innerHTML = obj.weather[0].description;
  document.getElementById("location_country").innerHTML =
    obj.name + ", " + obj.sys.country;
  num_temperature = obj.main.temp - 273.15;
  num_temperature = Math.round(num_temperature * 10) / 10;
  document.getElementById("temperature_openWeather").innerHTML =
    num_temperature + "°C";

  num_feesLike = obj.main.feels_like - 273.15;
  10000;
  num_feesLike = Math.round(num_feesLike * 10) / 10;
  document.getElementById("feelsLike_OpenWeather").innerHTML =
    num_feesLike + "°C";

  num_visibility = obj.visibility / 1000;

  document.getElementById("visibility_openWeather").innerHTML =
    num_visibility + " km/h";

  document.getElementById("humidity_openWeather").innerHTML =
    obj.main.humidity + " %";

  document.getElementById("icon_weather").src =
    "http://openweathermap.org/img/w/" + obj.weather[0].icon + ".png";

  if (request.status >= 200 && request.status < 400) {
    var temp = obj.main.temp;
  } else {
    console.log("The city doesn't exist! Kindly check");
  }
};
request.send();
//Date
var today = new Date();
var date =
  today.getDate() + "/" + (today.getMonth() + 1) + "/" + today.getFullYear();
var time = today.getHours() + "h:" + today.getMinutes() + "p";

document.getElementById("date_openWeather").innerHTML =
  "Thứ " + (today.getDay() + 1) + ", " + date;
document.getElementById("time_openWeather").innerHTML = time;

//Upload file
var imgName, imgUrl;
var files = [];
var reader = new FileReader();

document.getElementById("select").onclick = function () {
  var input = document.createElement("input");
  input.type = "file";
  input.onchange = (e) => {
    files = e.target.files;
    reader = new FileReader();
    reader.onload = function () {
      document.getElementById("myimg").style.display = "block";
      document.getElementById("myimg").src = reader.result;
      document.getElementById("icon_user").style.display = "none";
    };
    reader.readAsDataURL(files[0]);
  };
  input.click();
};

//Firebase
firebase.auth().onAuthStateChanged(function (user) {
  if (user) {
    // User is signed in.

    var user = firebase.auth().currentUser;

    if (user != null) {
      var email_id = user.email;
      document.getElementById("title_name_user1").innerHTML =
        "Welcome User : " + email_id;
    }
  }
});

function signOut() {
  firebase
    .auth()
    .signOut()
    .then(function () {
      window.location = "../View/login.html";
      // Sign-out successful.
    })
    .catch((error) => {
      // An error happened.
      var errorMessage = error.message;
      window.alert("error: " + errorMessage);
    });
}
