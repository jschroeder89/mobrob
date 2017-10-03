const electron = require('electron');
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;
var sensorGUI = null,
    pathGUI = null,
    sensorGUI_x_Pos = 0,
    sensorGUI_y_Pos = 0,
    pathGUI_x_Pos = 800,
    pathGUI_y_Pos = 0;
app.on('ready', function(){
    sensorGUI = new BrowserWindow({
        width: 800,
        height: 1024
    });
    sensorGUI.setPosition(sensorGUI_x_Pos, sensorGUI_y_Pos);
    //sensorGUI.webContents.openDevTools();
    sensorGUI.loadURL(`file://${__dirname}/app/sensorGUI.html`);
})

app.on('ready', function(){
    pathGUI = new BrowserWindow({
        width: 800,
        height: 800
    });
    pathGUI.setPosition(pathGUI_x_Pos, pathGUI_y_Pos);
    //pathGUI.webContents.openDevTools();
    pathGUI.loadURL(`file://${__dirname}/app/pathGUI.html`);
})
