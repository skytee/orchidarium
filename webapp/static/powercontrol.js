// postInterval keeps reference to refresh timer
var postInterval;

jQuery(document).ready(function() {

	// send request to backend when user clicks on checkbox
	jQuery(":checkbox").click(function() {
		var id = this.id;
		var formData = {};
		formData[id] = this.checked;
		updateForm(formData);
	});

	// toggle periodic refresh of displayed values
	jQuery(":checkbox#autoupdate").click(function() {
		if (this.checked) {
			postInterval = setInterval("updateForm()",1000);
		} else {
			clearInterval(postInterval);
		}
	});

	// work around browser keeping form data on page reload, i.e. checkbox checked
	jQuery(':checkbox#autoupdate').is(':checked') ? postInterval = setInterval("updateForm()",1000) : true ;

	// fill in form from backend when page become ready
	updateForm();
});

updateForm = function(formData) {
	jQuery.ajax(
	{
		type: "POST",
		cache: false,
		data: formData,
		success: function(data) {
			var json = jQuery.parseJSON(data);
			$("#socket1").prop('checked', json.socket1);
			$("#socket2").prop('checked', json.socket2);
			$("#socket3").prop('checked', json.socket3);
			$("#socket4").prop('checked', json.socket4);
			$("#sysmsg").html(json.sysmsg);
			$("#temperature").html(json.temperature);
			$("#humidity").html(json.humidity);
			$("#illuminance").html(json.illuminance);
		},
		error: function(e) {
			clearInterval(postInterval);
			alert("Can't connect to backend.");
			console.log(e.message);
		}
	});
	return false;
} 
