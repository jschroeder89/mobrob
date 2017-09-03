var front = document.getElementById("front");
var fillColor = "#4CAF50";
var testColor = "#000000";
var canvasArray = {
    canvas: []
};
//Front Sensor Canvas
var s0 = front.getContext("2d");
    s0.fillStyle = fillColor;
    s0.fillRect(0, 0, 150, 75);
    canvasArray.canvas.push(s0);

var s1 = front.getContext("2d");
    s1.fillStyle = fillColor;
    s1.fillRect(95, 0, 150, 75);

var s2 = front.getContext("2d");
    s2.fillStyle = fillColor;
    s2.fillRect(190, 0, 150, 75);

var s3 = front.getContext("2d");
    s3.fillStyle = fillColor;
    s3.fillRect(285, 0, 150, 75);

var s4 = front.getContext("2d");
    s4.fillStyle = fillColor;
    s4.fillRect(375, 0, 150, 75);

var s5 = front.getContext("2d");
    s5.fillStyle = fillColor;
    s5.fillRect(460, 0, 150, 75);

var s6 = front.getContext("2d");
    s6.fillStyle = fillColor;
    s6.fillRect(555, 0, 150, 75);

var s7 = front.getContext("2d");
    s7.fillStyle = fillColor;
    s7.fillRect(665, 0, 150, 75);

var s8 = front.getContext("2d");
    s8.fillStyle = fillColor;
    s8.fillRect(0, 75, 150, 75);

var s9 = front.getContext("2d");
    s9.fillStyle = fillColor;
    s9.fillRect(95, 75, 150, 75);

var s10 = front.getContext("2d");
    s10.fillStyle = fillColor;
    s10.fillRect(190, 75, 150, 75);

var s11 = front.getContext("2d");
    s11.fillStyle = fillColor;
    s11.fillRect(285, 75, 150, 75);

var s12 = front.getContext("2d");
    s12.fillStyle = fillColor;
    s12.fillRect(375, 75, 150, 75);

var s13 = front.getContext("2d");
    s13.fillStyle = fillColor;
    s13.fillRect(460, 75, 150, 75);

var s14 = front.getContext("2d");
    s14.fillStyle = fillColor;
    s14.fillRect(555, 75, 150, 75);

var s15 = front.getContext("2d");
    s15.fillStyle = fillColor;
    s15.fillRect(665, 75, 150, 75);

//Left Sensor Canvas
var left = document.getElementById("left");

var s16 = left.getContext("2d");
    s16.fillStyle = fillColor;
    s16.fillRect(0, 0, 75, 100);

var s17 = left.getContext("2d");
    s17.fillStyle = fillColor;
    s17.fillRect(0, 100, 75, 100);

var s18 = left.getContext("2d");
    s18.fillStyle = fillColor;
    s18.fillRect(0, 200, 75, 100);

var s19 = left.getContext("2d");
    s19.fillStyle = fillColor;
    s19.fillRect(0, 300, 75, 100);

var s20 = left.getContext("2d");
    s20.fillStyle = fillColor;
    s20.fillRect(75, 0, 75, 100);

var s21 = left.getContext("2d");
    s21.fillStyle = fillColor;
    s21.fillRect(75, 100, 75, 100);

var s22 = left.getContext("2d");
    s22.fillStyle = fillColor;
    s22.fillRect(75, 200, 75, 100);

var s23 = left.getContext("2d");
    s23.fillStyle = fillColor;
    s23.fillRect(75, 300, 75, 100);

//Right Sensor Canvas
var right = document.getElementById("right");

var s24 = right.getContext("2d");
    s24.fillStyle = fillColor;
    s24.fillRect(0, 0, 75, 100);

var s25 = right.getContext("2d");
    s25.fillStyle = fillColor;
    s25.fillRect(0, 100, 75, 100);

var s26 = right.getContext("2d");
    s26.fillStyle = fillColor;
    s26.fillRect(0, 200, 75, 100);

var s27 = right.getContext("2d");
    s27.fillStyle = fillColor;
    s27.fillRect(0, 300, 75, 100);

var s28 = right.getContext("2d");
    s28.fillStyle = fillColor;
    s28.fillRect(75, 0, 75, 100);

var s29 = right.getContext("2d");
    s29.fillStyle = fillColor;
    s29.fillRect(75, 100, 75, 100);

var s30 = right.getContext("2d");
    s30.fillStyle = fillColor;
    s30.fillRect(75, 200, 75, 100);

var s31 = right.getContext("2d");
    s31.fillStyle = fillColor;
    s31.fillRect(75, 300, 75, 100);

//Rear Sensor Canvas
var rear = document.getElementById("rear");

var s32 = rear.getContext("2d");
    s32.fillStyle = fillColor;
    s32.fillRect(0, 0, 150, 75);

var s33 = rear.getContext("2d");
    s33.fillStyle = fillColor;
    s33.fillRect(150, 0, 150, 75);

var s34 = rear.getContext("2d");
    s34.fillStyle = fillColor;
    s34.fillRect(300, 0, 150, 75);

var s35 = rear.getContext("2d");
    s35.fillStyle = fillColor;
    s35.fillRect(450, 0, 150, 75);

var s36 = rear.getContext("2d");
    s36.fillStyle = fillColor;
    s36.fillRect(0, 75, 150, 75);

var s37 = rear.getContext("2d");
    s37.fillStyle = fillColor;
    s37.fillRect(150, 75, 150, 75);

var s38 = rear.getContext("2d");
    s38.fillStyle = fillColor;
    s38.fillRect(300, 75, 150, 75);

var s39 = rear.getContext("2d");
    s39.fillStyle = fillColor;
    s39.fillRect(450, 75, 150, 75);

    <canvas id="front" class="front" width="760" height="150"
        style="border:4px solid #c3c3c3;">
    </canvas>

    <canvas id="left" class="left" width="150" height="400"
        style="border:4px solid #c3c3c3;">
    </canvas>

    <canvas id="right" class="right" width="150" height="400"
        style="border:4px solid #c3c3c3;">
    </canvas>

    <canvas id="rear" class="rear" width="600" height="150"
        style="border:4px solid #c3c3c3;">
    </canvas>

    <script src="canvas.js"></script>
