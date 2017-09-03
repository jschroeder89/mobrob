var trace1 = {
    x: [0, 1, 2, 3, 4, -1, -3, -5],
    y: [0, 1, 2, 3, 4, -2, 3, -4],
    mode: 'lines+markers',
    marker: {
        color: 'rgb(0, 121, 241)',
        size: 8,

    },
    line: {
        color: 'rgb(0, 121, 241)',
        width: 4
    }
};

var layout = {
    title: 'Path Tracking',
    xaxis: {
        title: 'x-Position'
    },
    yaxis: {
        title: 'y-Position'
    },
    width: 550,
    height: 550
};

var data = [trace1];
Plotly.newPlot('path', data, layout);
