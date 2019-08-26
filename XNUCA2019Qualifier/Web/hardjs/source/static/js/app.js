class Viewer {
	constructor(container) {

			this.allNode = {}
			this.container = container;
			this.sandbox = document.createElement('iframe');
			this.sandbox.setAttribute('src', '/sandbox');
			this.sandbox.setAttribute('height',"100%");
			this.sandbox.setAttribute('weight',"100%");
			this.sandbox.setAttribute('sandbox', 'allow-same-origin');
			this.sandbox.setAttribute('class', 'sandbox');
			this.sandboxLoaded = new Promise((resolve) => {
				this.sandbox.addEventListener('load', resolve);
			});
		this.sandboxLoaded.then(() => initListener(this.sandbox.contentWindow));
		this.container.appendChild(this.sandbox);
	};

	async render(){	
		await this.sandboxLoaded;
		this.sandbox.contentWindow.postMessage({
			cmd:"render",
			dom:this.allNode
		},origin);
	}
	
}


function getAll(allNode){

	$.ajax({
		url:"/get",
		type:"get",
		async:false,
		success: function(datas){
			for(var i=0 ;i<datas.length; i++){
				$.extend(true,allNode,datas[i])
			}
			// console.log(allNode);
		}
	})
}



function initListener(sandboxWindow) {
    with (sandboxWindow) {
    addEventListener('message', (evt) => {

		if (evt.origin == origin) {
			const viewport = document.querySelector('#viewport');

			if(evt.data.cmd == 'render' ){
				
				while (viewport.children[0]) {
					viewport.children[0].remove();
				}

				var dom = evt.data.dom ;
				for (key in dom){
					switch(key){
						case 'header':
							$tmp = $("li[type='header']");
							$newNode = $( $tmp.html() );
							$newNode.find("span.content").html(dom[key][0]);

							// console.log($newNode.html());

							viewport.appendChild( $newNode[0] );
							break;
						case "notice":
							// console.log(dom[key]);
							$tmp = $("li[type='notice']");
							$newNode = $( $tmp.html() );
							$newNode.find("span.content").html(dom[key][0]);
							viewport.appendChild( $newNode[0] );

							break;
						case "wiki":
							// console.log(dom[key]);

							$tmp = $("li[type='wiki']");
							$newNode = $( $tmp.html() );
							$newNode.find("span.content").html(dom[key][0]);
							viewport.appendChild( $newNode[0] );

							break;
						case "button":
							// console.log(dom[key]);

							$tmp = $("li[type='button']");
							$newNode = $( $tmp.html() );
							$newNode.find("span.content").html(dom[key][0]);
							viewport.appendChild( $newNode[0] );

							break;
						case "message":
							// console.log(dom[key]);
							$tmp = $("li[type='message']");
							$newNode = $( $tmp.html() );

							$newNode.find("span.content").html(dom[key][0]);
							viewport.appendChild( $newNode[0] );
							break;
						default:
							console.log(key,":",dom[key]);
					}

				}
				
			}
        }
      });
    }
}


$(document).ready(function($) {

	const viewer = new Viewer(document.querySelector('#viewer'));    
	getAll(viewer.allNode);
	viewer.render();

	// init hint 
	(function(){
		var hints = {
			header : "自定义内容",
			notice: "自定义公告",
			wiki : "自定义wiki",
			button:"自定义内容",
			message: "自定义留言内容"
		};
		for(key in hints){
			// console.log(key);
			element = $("li[type='"+key+"']"); 
			if(element){
				element.find("span.content").html(hints[key]);
			}
		}
	})();

	// 点击元素
	$(".element").click(function() {
		// 获取登录窗体代码
		var type = $(this).attr('type');
		var loginHtml = $("#loginHtml").html();
		showLayer(loginHtml, 500, 300, closeCallback);

		$("#SubmitBtn").click(function() {
			
			var info = $("input[name='info']").val();
			if( type && info ) {
				$.ajax({
					url:"/add",
					type:"POST",
					data: {
						type: type,
						content: info
					},
					success: function(data){
						$.extend(true,viewer.allNode,data);
						viewer.render();
						hideLayer();
						// 关闭的回调函数
						closeCallback();
						// console.log(data);
					}

				});
				alert("添加成功");

			} else {
				$(".error-msg").html("请输入正确的信息");

			}
		});
	});
	
	// 弹出层关闭回调函数
	function closeCallback(){
		$(".error-msg").html("");
	}
	
	// 显示弹出层
	function showLayer(html,width,height,closeCallback){
		// 显示弹出层遮罩
		$("#layer-mask").show();
		// 显示弹出层窗体
		$("#layer-pop").show();
		// 设置弹出层窗体样式
		$("#layer-pop").css({
			width : width,
			height : height
		});
		// 填充弹出层窗体内容
		$("#layer-content").html(html);
		// 弹出层关闭按钮绑定事件
		$("#layer-close").click(function(){
			// 弹出层关闭
			hideLayer();
			// 关闭的回调函数
			closeCallback();
		});
	}

	// 隐藏弹出层
	function hideLayer(){
		// 弹出层关闭
		$("#layer-mask").hide();
		$("#layer-pop").hide();
	}


});


