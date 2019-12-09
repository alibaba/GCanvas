 var tw = ctx.measureText("USA");
 var mw = tw.width;

 ctx.fillStyle = "#ff0000";
 ctx.font = 12*ratio+"px Arial-MT";
 ctx.fillText("USA", 10*ratio, 50*ratio);
 ctx.fillText("文字怎么样 !!! width:" + mw, 20*ratio, 100*ratio);


 ctx.fillStyle = '#FF0000';
 ctx.font = 25*ratio+"px 微软雅黑";
 ctx.fillText("w3school.com.cn", 35*ratio, 50*ratio);