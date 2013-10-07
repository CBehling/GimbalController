//create canvas and get context reference
var canvas = new Array();
var context = [];
var width,height,radius;
var numLayers;
var x, y;

/* All Layers */
function initCanvas(){
  numLayers=$(".gimbal").length;

  console.log(numLayers);
  for (var i=0; i<numLayers; i++){
    console.log($("#gimbalLayer"+i)[0]);
    canvas.push($("#gimbalLayer"+i)[0]);
    console.log(canvas);
    context.push(canvas[i].getContext('2d'));
    canvas[i].style.zIndex=i;
  }
  width = canvas[0].width;
  height = canvas[0].height;
  radius = Math.min(width,height)/2;
}

/* Background Layer */
function drawBackground(){
  context[0].beginPath();
  context[0].arc(width/2,height/2,radius,0,2*Math.PI,false);
  context[0].stroke();
}

/* Data Layer */

function rgb2Hex(red, green, blue)
{
    var color = red + 256 * green + 65536 * blue + 16777216;
    color = color.toString(16).substring(1,7);
    return '#' + color;
}

function addData(x,y,val){
  if (val > 2000){
    val = 256*(1-(val-2000)/7000);
    console.log("Value: "+val);
  }
  val = Math.round(val);
  console.log("Value: "+val);
  context[1].beginPath();
  context[1].arc(x, y, 3, 0, 2 * Math.PI, false);
  context[1].fillStyle = rgb2Hex(0,Math.round(val*.66),val);
  context[1].fill();
}

function clearLayer(n) {
  context[n].clearRect ( 0 , 0 , width , height );
}

/* Position Layer */
function clickMove(evt){
  x = evt.offsetX;
  y = evt.offsetY;
  var r = Math.sqrt((x-width/2)*(x-width/2) + (y-height/2)*(y-height/2));
  if (r>radius){
    return false;
  }
  moveTo(x,y);//update graphical position

  //send AJAX request
  var temp = getPanTiltFromXY(x, y);
  $.ajax({url:"/_position", datatype:'json', data:{
    'pan':temp[0],
    'tilt':temp[1]
  }})
  //TODO send AJAX request
  console.log('X:    ' + x);
  console.log('Y:    ' + y);
  console.log('Pan:  ' + temp[0]);
  console.log('Tilt: ' + temp[1]);
}

function moveTo(x,y){
  clearLayer(2);
  context[2].beginPath();
  context[2].arc(x,y,3,0,2*Math.PI,false);
  context[2].stroke();
}

/* Scan */
function ping(x,y){

  var temp = getPanTiltFromXY(x,y);
  $.ajax({method:"GET",
    url:"/_ping",
    datatype:"json",
    data:{
      'pan':temp[0],
      'tilt':temp[1]
    },
    success: function(data){
      var temp = getXYFromPanTilt(data.pan, data.tilt);
      //TODO: convert data to range 0,255
      addData(temp[0], temp[1], data.dist);

      temp = getNextInRange(temp[0],temp[1]);
      if (temp != undefined){
        ping(temp[0],temp[1]);
      }
    }
  });
}

function getNextInRange(x,y){
  do{
    if (x>width){
      return undefined;
    }
    if (y>height){
      y -= 10;
      x += 10;
      continue;
    }
    if (y<0){
      y+=10;
      x+=10;
      continue;
    }
    if (!(Math.round(x/10)%2)){
      y+=10; 
    }else{
      y-=10;
    }
  }while(!inRange(x,y));
  return [x,y];
}

function scan(){
  var temp = getNextInRange(0,0);
  ping(temp[0],temp[1]);
  /*
  for (var x=0; x<width; x+=30){
    for (var y=0; y<height; y+=30){
      if (inRange(x,y)){
        ping(x,y);
        console.log("ping x: " + x+ " y: " + y)
      }
    }
  }*/
}

/* Helper Functions */
function getXYFromPanTilt(pan,tilt){
  var r, theta;
  if (tilt <= Math.PI/2){
    theta = Math.PI/2-pan;
    r = radius*Math.cos(tilt);
  }else{
    theta = 3*Math.PI/2 - pan;
    r = radius*Math.cos(Math.PI-tilt);
  }
  var x = r*Math.cos(theta) + width/2;
  var y = r*Math.sin(theta) + height/2;
  return [x,y]
}

function getPanTiltFromXY(x,y){
  //move origin
  x -= width/2;
  y -= height/2;

  //calculate tilt
  var r = Math.sqrt(x*x + y*y);
  if (x>=0){
    tilt = Math.acos(r/radius);
  }else{
    tilt = Math.PI - Math.acos(r/radius);
  }
  
  //calculate pan
  var pan = Math.PI/2-Math.atan(y/x);
  
  return [pan, tilt]
}

function inRange(x,y){
  x -= width/2;
  y -= height/2;
  return x*x + y*y <= radius*radius;
}

/* Initialize Everything */
$(function(){
  initCanvas();
  drawBackground();

  $(canvas[canvas.length-1]).bind("draginit",clickMove);
  $(canvas[canvas.length-1]).bind("drag",clickMove);
    //test displaying datatype
  $("#scan").bind("click",scan);
});