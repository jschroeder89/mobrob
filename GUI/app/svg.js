var nonCollisionColor = "#00FF50"
var maxCollisionColor = "#FF0050"
var nonCollisionColorValue = 65360
var fillColor
var testValue = 2983
var dezVal = testValue + nonCollisionColorValue
var hexStr = parseInt(testValue + nonCollisionColorValue, 16)
console.log(dezVal, hexStr);
var sensorfieldStr = "sensorfield_"
var sensorArray = []
for (var i = 0; i < 40; i++) {
    sensorArray.push(sensorfieldStr.concat(String(i)));
    document.getElementById(sensorArray[i]).style.fill = "#426819";
}
