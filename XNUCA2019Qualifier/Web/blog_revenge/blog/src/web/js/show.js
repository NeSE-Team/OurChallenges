function main() {
    var id = location.search.match('id=(.*)');
    var id = id ? id[1] : '1';
    document.write('<script src="/main/jsonp?id='+id+ '&callback=shownote"></script>');
}

function shownote(data){
    // console.log(data);
      if(data.title){
        // console.log(data.title);  
        $('#title').html("title:"+data.title);
      }
      if(data.content){
        //   console.log(data.content);
          $('#content').html("content:"+data.content);
      }
      
}
main();