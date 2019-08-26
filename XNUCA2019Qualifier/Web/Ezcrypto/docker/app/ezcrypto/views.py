from django.http import HttpResponse
from ezcrypto.models import User
from ezcrypto.models import Record
from django.shortcuts import render
from django.views.decorators.http import require_http_methods
from ezcrypto.common import *
from ezcrypto.settings import FLAG
from ezcrypto.settings import TIMES


@require_http_methods(['GET'])
def index(request):
    user = request.session.get('user')
    if user is None:
        return render(request, 'index.html')
    else:
        context = {
            'user':  user,
            'luky': request.session.get('luky'),
            'root_flag': request.session.get('root_flag'),
            'user_flag': request.session.get(user + '_flag'),
            'times': request.session.get('times'),
        }
        return render(request, 'user.html', context)


@require_http_methods(['POST'])
def change(request):
    user = request.session.get('user')
    if user is None:
        return render(request, 'index.html')
    else:
        userone = User.objects.filter(username=user)[0]
        times = userone.times
        if times > 0:
            userone.times = times - 1
            message = request.POST.get('message')
            luky = request.POST.get('luky')
            if (len(message) > 300) or (len(luky) > 300):
                content = "<script>alert('明文和幸运数字不能过长');window.location='/';</script>"
                return HttpResponse(content)
            if (message is None) or (luky is None):
                content = "<script>alert('明文和幸运数字不能为空');window.location='/';</script>"
                return HttpResponse(content)
            if (wafs(message) is not None) or (wafs(luky) is not None):
                content = "<script>alert('检测到危险');window.location='/';</script>"
                return HttpResponse(content)
            records = Record.objects.extra(
                where=['username=%s', 'message!=%s', 'luky!={0}'.format(luky)],
                params=[user, message]
            )
            if len(records) == 0:
                content = "<script>alert('必须修改明文和幸运数字');window.location='/';</script>"
                return HttpResponse(content)
            recordone = records[0]
            root_flag = crypt(str(luky) + FLAG, request.session['root_N'], request.session['root_E'])
            user_flag = crypt(message, request.session[recordone.username + '_N'], request.session[recordone.username + '_E'])
            recordone.message = message
            try:
                recordone.luky = int(luky)
            except:
                recordone.luky = 47
            recordone.secretroot = str(root_flag)
            recordone.secretuser = str(user_flag)
            request.session['luky'] = luky
            request.session['root_flag'] = root_flag
            request.session[recordone.username + '_flag'] = user_flag
            request.session['times'] = times - 1
            userone.save()
            recordone.save()
            content = "<script>alert('修改成功');window.location='/';</script>"
            return HttpResponse(content)
        else:
            userone.times = TIMES
            message = request.POST.get('message')
            luky = request.POST.get('luky')
            if (len(message) > 300) or (len(luky) > 300):
                content = "<script>alert('明文和幸运数字不能过长');window.location='/';</script>"
                return HttpResponse(content)            
            if (message is None) or (luky is None):
                content = "<script>alert('明文和幸运数字不能为空');window.location='/';</script>"
                return HttpResponse(content)
            if (wafd(message) is not None) or (wafd(luky) is not None):
                content = "<script>alert('检测到危险');window.location='/';</script>"
                return HttpResponse(content)
            if (luky.count(')') > 1) or (luky.count('(') > 1):
                content = "<script>alert('检测到危险');window.location='/';</script>"
                return HttpResponse(content)
            records = Record.objects.extra(
                where=['username=%s', 'message!=%s', 'luky!={0}'.format(luky)],
                params=[user, message]
            )
            if len(records) == 0:
                content = "<script>alert('必须修改明文和幸运数字');window.location='/';</script>"
                return HttpResponse(content)
            recordone = records[0]
            (Nroot, Eroot) = create_key(0.32, 0.35)
            root_flag = crypt(FLAG, Nroot, Eroot)
            userone.Nroot = Nroot
            userone.Eroot = Eroot
            userone.save()
            (Nuser, Euser) = create_key(recordone.lowlimit, recordone.uplimit)
            user_flag = crypt(message, Nuser, Euser)
            recordone.message = message
            try:
                recordone.luky = int(luky)
            except:
                recordone.luky = 47
            recordone.secretroot = str(root_flag)
            recordone.secretuser = str(user_flag)
            recordone.Nuser = str(Nuser)
            recordone.Euser = str(Euser)
            request.session['luky'] = luky
            request.session['root_N'] = Nroot
            request.session['root_E'] = Eroot
            request.session['root_flag'] = root_flag
            request.session[recordone.username + '_N'] = Nuser
            request.session[recordone.username + '_E'] = Euser
            request.session[recordone.username + '_flag'] = user_flag
            request.session['times'] = TIMES
            recordone.save()
            content = "<script>alert('修改成功');window.location='/';</script>"
            return HttpResponse(content)


@require_http_methods(['POST'])
def login(request):
    username = request.POST.get('username')
    password = request.POST.get('password')
    if (username is None) or (password is None):
        content = "<script>alert('用户名和密码不能为空');window.location='/';</script>"
        return HttpResponse(content)
    if username == 'root':
        content = "<script>alert('用户名不能为root');window.location='/';</script>"
        return HttpResponse(content)
    if len(username) > 30:
        content = "<script>alert('用户名过长');window.location='/';</script>"
        return HttpResponse(content)
    user = User.objects.filter(username=username)
    if len(user) == 0:
        password = md5(password)
        lowlimit = 0.32
        uplimit = 0.35
        (Nroot, Eroot) = create_key(lowlimit, uplimit)
        User.objects.create(username=username, password=password, times=TIMES, Nroot=str(Nroot), Eroot=str(Eroot))
        secretroot = crypt(FLAG, Nroot, Eroot)
        (Nuser, Euser) = create_key(lowlimit, uplimit)
        secretuser = crypt("welcome to RSA", Nuser, Euser)
        message = "welcome to RSA"
        luky = 47
        Record.objects.create(lowlimit=lowlimit, uplimit=uplimit, username=username, secretroot=str(secretroot), Nuser=str(Nuser), Euser=str(Euser), secretuser=str(secretuser), message=message, luky=luky)
        request.session['user'] = username
        request.session['luky'] = luky
        request.session['root_N'] = Nroot
        request.session['root_E'] = Eroot
        request.session['root_flag'] = secretroot
        request.session[username + '_N'] = Nuser
        request.session[username + '_E'] = Euser
        request.session[username + '_flag'] = secretuser
        request.session['times'] = TIMES
        content = "<script>alert('登录成功');window.location='/';</script>"
        return HttpResponse(content)
    else:
        userone = user[0]
        recordone = Record.objects.filter(username=username)[0]
        auth = userone.password
        if md5(password) == auth:
            request.session['user'] = username
            request.session['luky'] = recordone.luky
            request.session['root_N'] = userone.Nroot
            request.session['root_E'] = userone.Eroot
            request.session['root_flag'] = recordone.secretroot
            request.session[username + '_N'] = recordone.Nuser
            request.session[username + '_E'] = recordone.Euser
            request.session[username + '_flag'] = recordone.secretuser
            request.session['times'] = recordone.times
            content = "<script>alert('登录成功');window.location='/';</script>"
            return HttpResponse(content)
        else:
            content = "<script>alert('用户名密码错误');window.location='/';</script>"
            return HttpResponse(content)