
/* function begin */
function addRow(user_answer, chosen_pair){
  if(user_answer.toLowerCase() == chosen_pair.capital.toLowerCase()){ //correct answer
    $("tbody").append('<tr class="blueRow"><td id="cor_country"></td><td id="user_answer"></td><td id="fa_check"></td></tr>');
    $("#fa_check").html('<i class="fas fa-check"></i><button class="deleteBtn" onclick="deleteRow(this)">Delete</button>');
    $("#cor_country").text(chosen_pair.country);
    $("#user_answer").text(chosen_pair.capital);

    $("#fa_check").removeAttr('id');
    $("#cor_country").removeAttr('id');
    $("#user_answer").removeAttr('id');

    if( $("#wrong_check").is(":checked") ){
      $("#all_check").prop('checked', true);
    }
  }
  else{ //wrong answer
    $("tbody").append('<tr class="redRow"><td id="wrong_country"></td><td><s id="user_answer"><s/></td><td id="cor_capital"></td></tr>');
    $("#wrong_country").text(chosen_pair.country);
    $("#user_answer").text(user_answer);
    $("#cor_capital").html(chosen_pair.capital + '<button class="deleteBtn" onclick="deleteRow(this)">Delete</button>');

    $("#wrong_country").removeAttr('id');
    $("#user_answer").removeAttr('id');
    $("#cor_capital").removeAttr('id');

    if( $("#cor_check").is(":checked") ){
      $("#all_check").prop('checked', true);
    }
  }

  $("#pr2__answer").val('');
}

function deleteRow(obj){
  var tr = $(obj).parent().parent();

  tr.remove();

  //check if tr is empty
  if( $("#all_check").is(":checked") ){ //All
    if( $("tr").hasClass('redRow') || $("tr").hasClass('blueRow') ){
      $(".emptyRow").hide();
    }
    else{
      $(".emptyRow").show();
    }
  }
  else if( $("#cor_check").is(":checked") ){ //Correct Only
    if( $("tr").hasClass('blueRow') ){
      $(".emptyRow").hide();
    }
    else{
      $(".emptyRow").show();
    }
  }
  else if( $("#wrong_check").is(":checked") ){ //Wrong Only
    if( $("tr").hasClass('redRow') ){
      $(".emptyRow").hide();
    }
    else{
      $(".emptyRow").show();
    }
  }
}
/* function end */

// This allows the Javascript code inside this block to only run when the page
// has finished loading in the browser.
$( document ).ready(function() {
  var country_capital_pairs = pairs
  var chosen_pair = country_capital_pairs[Math.floor(Math.random() * 206)]; //0~205
  var capital = new Array();
  var auto_complete = false;

  //page load
  $("#pr2__question").text(chosen_pair.country);

  //submit button click event
  $("#pr2__submit").click(function(){

    if( $(".emptyRow").is($(".emptyRow").show()) ){
      $(".emptyRow").hide();
    }
    
    //check answer
    addRow($("#pr2__answer").val(), chosen_pair);

    //reload question
    chosen_pair = country_capital_pairs[Math.floor(Math.random() * 206)]; //0~205
    $("#pr2__question").text(chosen_pair.country);
  });

  //press Enter(not autocomplete)
  $("#pr2__answer").keydown(function(key){
  	if(key.keyCode == 13){
      if(!auto_complete){
        $("#pr2__submit").click();
      }  		
      else auto_complete = false;
    }
  });

  //autocomplete
  for (var i = 0; i < country_capital_pairs.length; i++) {
    capital[i] = country_capital_pairs[i].capital;
  }
  
  $("#pr2__answer").autocomplete({
    source: capital,
    minLength: 2,
    focus: function(event, ui){
      auto_complete = true;
    },
    select: function(event, ui){   
      if( $(".emptyRow").is($(".emptyRow").show()) ){
        $(".emptyRow").hide();
      }
      //check answer
      addRow(ui.item.value, chosen_pair);
      //reload question
      chosen_pair = country_capital_pairs[Math.floor(Math.random() * 206)]; //0~205
      $("#pr2__question").text(chosen_pair.country);
      return false; //empty input value
    },
    close: function(event){
      auto_complete = false;
    }
  });

  //check radio button
  $(".radioBtn").click(function(){
    if( $("#all_check").is(":checked") ){ //All
      if( $(".blueRow").is($(".blueRow").hide()) ){
        $(".blueRow").show();
      }
      if( $(".redRow").is($(".redRow").hide()) ){
        $(".redRow").show();
      }

      if( $("tr").hasClass('redRow') || $("tr").hasClass('blueRow') ){
        $(".emptyRow").hide();
      }
      else{
        $(".emptyRow").show();
      }
    }
    else if( $("#cor_check").is(":checked") ){ //Correct Only
      if( $(".blueRow").is($(".blueRow").hide()) ){
        $(".blueRow").show();
      }
      if( $(".redRow").is($(".redRow").show()) ){
        $(".redRow").hide();
      }

      if( $("tr").hasClass('blueRow') ){
        $(".emptyRow").hide();
      }
      else{
        $(".emptyRow").show();
      }
    }
    else if( $("#wrong_check").is(":checked") ){ //Wrong Only
      if( $(".blueRow").is($(".blueRow").show()) ){
        $(".blueRow").hide();
      }
      if( $(".redRow").is($(".redRow").hide()) ){
        $(".redRow").show();
      }

      if( $("tr").hasClass('redRow') ){
        $(".emptyRow").hide();
      }
      else{
        $(".emptyRow").show();
      }
    }
  });  
});
