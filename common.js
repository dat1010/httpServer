function loadAndSetActive(file,element, active){

// first remove all other active classes
$("#Home").removeClass("active");
$("#About").removeClass("active");

$(active).addClass("active");
$(element).load(file);

return;

}

function homePage()
{
  $( "#pageBody" ).load( "home.html" );
  $("#aboutP").removeClass("active");
  $("#postP").removeClass("active");
  $("#homeP").addClass("active");
}

function aboutPage()
{
  $( "#pageBody" ).load( "about.html" );
  $("#homeP").removeClass("active");
  $("#postP").removeClass("active");
  $("#aboutP").addClass("active");
}

function postPage()
{
  $( "#pageBody" ).load( "post.html" );
  $("#homeP").removeClass("active");
  $("#aboutP").removeClass("active");
  $("#postP").addClass("active");	
}
