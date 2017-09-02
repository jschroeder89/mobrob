const electron = require('electron');
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;
var sensorGUI = null;
var sensorGUI_x_Pos = 0;
var sensorGUI_y_Pos = 0;
var pathGUI = null;
var pathGUI_x_Pos = 800;
var pathGUI_y_Pos = 0;
app.on('ready', function(){
    sensorGUI = new BrowserWindow({
        width: 800,
        height: 600
    });
    sensorGUI.setPosition(sensorGUI_x_Pos, sensorGUI_y_Pos);
    sensorGUI.webContents.openDevTools();
    sensorGUI.loadURL(`file://${__dirname}/app/sensorGUI.html`);
})

app.on('ready', function(){
    pathGUI = new BrowserWindow({
        width: 600,
        height: 600
    });
    pathGUI.setPosition(pathGUI_x_Pos, pathGUI_y_Pos);
    pathGUI.webContents.openDevTools();
    pathGUI.loadURL(`file://${__dirname}/app/pathGUI.html`);
})
