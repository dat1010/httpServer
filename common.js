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
  $("#homeP").addClass("active");
}

function aboutPage()
{
  $( "#pageBody" ).load( "about.html" );
  $("#homeP").removeClass("active");
  $("#aboutP").addClass("active");

}
