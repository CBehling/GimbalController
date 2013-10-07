//create canvas and get context reference
var canvas = new Array();
var context = [];
var width,height,radius;
var numLayers;

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
  context[1].beginPath();
  context[1].arc(x, y, 3, 0, 2 * Math.PI, false);
  context[1].fillStyle = rgb2Hex(0,0,val);
  context[1].fill();
}

function clearData () {
  //TODO: clear the entire layer
}

/* Position Layer */
function moveTo(x,y){
  context[2].clearRect ( 0 , 0 , width , height );
  context[2].beginPath();
  context[2].arc(x,y,3,0,2*Math.PI,false);
  context[2].stroke();
}
//TODO draw something that indicates the current position of the gimbal

/* Helper Functions */
function getXYFromPanTilt(pan,tilt){
  var r = Math.cos(tilt)*canvas.radius;
  var theta = pan;
  var x = r*Math.cos(theta);
  var y = r*Math.sin(theta);
  return [x,y]
}

function getPanTiltFromXY(x,y){
  //move origin
  x -= width/2;
  y -= height/2;

  //calculate tilt
  var r = Math.sqrt(x*x + y*y);
  var sign = y/Math.abs(y);
  if (y==0){
    sign = 0;
  }
  if (r>radius){
    r = radius;
  }
  var tilt = Math.acos(r/radius*sign);
  
  //calculate pan
  var pan = Math.atan(y/x)+Math.PI/2;
  
  return [pan, tilt]
}

/* Initialize Everything */
$(function(){
  initCanvas();
  drawBackground();

  $(canvas[canvas.length-1]).bind("click",function(evt){
    var x = evt.offsetX, y = evt.offsetY;
    var r = Math.sqrt((x-width/2)*(x-width/2) + (y-height/2)*(y-height/2));
    if (r>radius){
      return;
    }
    moveTo(x,y);//update graphical position

    //send AJAX request
    var temp = getPanTiltFromXY(x, y);
    $.ajax({url:"/_position", datatype:'json', data:{
      'pan':temp[0],
      'tilt':temp[1]
    }})
    //TODO send AJAX request
    console.log('Pan:  ' + temp[0]);
    console.log('Tilt: ' + temp[1]);
  });
    //test displaying data
    addData(100,100,200)
});