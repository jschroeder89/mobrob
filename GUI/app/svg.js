var nonCollisionColor = "#00FF00"
var maxCollisionColor = "#FF0000"

function valueToHexStringColor(val) {
    val = Math.round(val / 16);
    valRev = 255 - val;
    val = val.toString(16);
        if (val.length == 1) {
            val = val + "0";
        }
    valRev = valRev.toString(16);
        if (valRev.length == 1) {
            valRev = valRev + "0";
        }
    colorHexString = "#" + val + valRev + "0F";
    return colorHexString;
}

var fillColor = valueToHexStringColor(1000);

var sensorfieldStr = "sensorfield_"
var sensorArray = [];
for (var i = 0; i < 40; i++) {
    sensorArray.push(sensorfieldStr.concat(String(i)));
    document.getElementById(sensorArray[i]).style.fill = fillColor;
}
