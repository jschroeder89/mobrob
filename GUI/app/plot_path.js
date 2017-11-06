var trace1 = {
    x: [],
    y: [],
    mode: 'lines+markers',
    type: 'scattergl',
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
        title: 'x-Position',
        autorange: false
    },
    yaxis: {
        title: 'y-Position',
        autorange: false
    },
    width: 750,
    height: 750
};

var data = [trace1];
