ctx.fillStyle = "#ff0000";

    var dashes = [10, 5];
    ctx.setLineDash(dashes);
    ctx.lineWidth = 5*ratio;
    ctx.lineCap = "butt";
    ctx.beginPath();
    ctx.moveTo(30*ratio, 100*ratio);
    ctx.lineTo(500*ratio, 100*ratio);
    ctx.stroke();

    var dashes2 = ctx.getLineDash();
    dashes2.push(30);
    ctx.setLineDash(dashes2);

    ctx.beginPath();
    ctx.moveTo(30*ratio, 200*ratio);
    ctx.lineTo(500*ratio, 200*ratio);
    ctx.stroke();