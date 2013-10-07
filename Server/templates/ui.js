$(function() {
	$( "#pan" ).slider({
		min:0,
		max:65535,
		value: 32768,
		range: "min",
		animate: true,
		orientation: "vertical"
	}).bind('slide', function(event,ui){
		$.ajax({url:"/_position",datatype:'json',data:{
			'tilt':$("#tilt").slider("option","value"),
			'pan':$("#pan").slider("option","value")
		}})
	});
	$( "#tilt" ).slider({
		min:0,
		max:65535,
		value: 32768,
		orientation: "horizontal",
		range: "min",
		animate: true
	}).bind('slide', function(event,ui){
		$.ajax({url:"/_position",datatype:'json',data:{
			'tilt':$("#tilt").slider("option","value"),
			'pan':$("#pan").slider("option","value")
		}})
	});
	$("#scan").button().click(function(event){
		$.ajax({url:"/_scan"})
	});
});