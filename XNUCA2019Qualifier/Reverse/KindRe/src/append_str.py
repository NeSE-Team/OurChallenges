#coding: utf-8

s = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_{}'
#各字符串的频率
list_f = [22, 4, 11, 26, 44, 26, 7, 24, 20, 17, 1, 22, 11, 20, 20, 18, 1, 30, 11, 47, 30, 4, 3, 11, 3, 4, 11, 1, 2, 33, 10, 5, 2, 7, 2, 1, 1, 20, 8, 3, 4, 20, 10, 9, 13, 11, 8, 16, 12, 19, 7, 6, 25, 11, 3, 11, 7, 3, 1, 1, 3, 4, 41, 2, 4]
assert len(s)==len(list_f)
dict = {}
for i in xrange(len(s)):
    if list_f[i] in dict:
        dict[list_f[i]].append(s[i])
    else:
        dict[list_f[i]] = [s[i]]
print dict
new_f = sorted(list_f)
print new_f
add_nums = {}
add_nums[new_f[0]] = [0]
for i in xrange(1, len(new_f)):
    add_num = 0
    while (new_f[i]+add_num)<=new_f[i-1]:
        add_num+=1
    if new_f[i] in add_nums:
        add_nums[new_f[i]].append(add_num)
    else:
        add_nums[new_f[i]] = [add_num]
    new_f[i]+=add_num
print add_nums
print new_f
append_str = ''
for key in dict:
    for i in xrange(len(dict[key])):
        append_str+=dict[key][i]*add_nums[key][i]
print append_str
