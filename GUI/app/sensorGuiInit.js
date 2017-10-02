var sensorfieldStr = "sensorfield_"
var sensorArray = [];

function initializeSvgs() {
    for (var i = 0; i < 40; i++) {
        sensorArray.push(sensorfieldStr.concat(String(i)));
    }
}

function colorizeSensorField(field, value) {
    document.getElementById(sensorArray[field]).style.fill = value;
}

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
