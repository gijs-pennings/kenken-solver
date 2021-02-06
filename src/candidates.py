import functools
import itertools

MAX_CAGE = 5
MAX_VALUE = 9

values = range(1, MAX_VALUE + 1)
pairs = list(itertools.product(values, values))

def fill_to_square(mat):
    length = max([len(a) for a in mat])
    for a in mat: a.extend([0] * (length - len(a)))

def find(mat, op):
    for size in range(len(mat), MAX_CAGE + 1):
        arr = []
        stack = [[]]
        while stack: # while not empty ...
            a = stack.pop()
            for i in values:
                # Even in a staircase pattern, that can contain the greatest
                # number of duplicates, neighboring values cannot be equal.
                if len(a) > 0 and i == a[-1]: continue
                b = a + [i]
                if len(b) == size:
                    x = op(b)
                    if x >= len(arr): arr.extend([0] * (x - len(arr) + 1))
                    arr[x] |= to_mask(b)
                else:
                    stack.append(b)
        mat.append(arr)

def find_size2(lst, op):
    for n in range(len(lst), MAX_VALUE + 1):
        c = flatten([(i,j) for i,j in pairs if op(i,j) == n])
        lst.append(to_mask(c))

def flatten(lst):
    return itertools.chain.from_iterable(lst)

def to_mask(lst):
    return functools.reduce(lambda m,x: m | (1 << x), lst, 0)

def to_string(mat):
    items = [to_string(x) if type(x) == list else str(x) for x in mat]
    return '{ ' + ', '.join(items) + ' }'

add = [[]]    # [size][target]
sub = [0]     # [target] since always size == 2
mul = [[]]    # like add
div = [0, 0]  # like sub

find(add, lambda arr: sum(arr))
find(mul, lambda arr: functools.reduce(lambda x,y: x*y, arr))
find_size2(sub, lambda x,y: abs(x-y))
find_size2(div, lambda x,y: max(x,y) / min(x,y))

fill_to_square(add)
fill_to_square(mul)
sub.pop() # always zero

print(f'int candidatesAdd[][{len(add[0])}] {to_string(add)};')
print(f'int candidatesSub[] {to_string(sub)};')
print(f'int candidatesMul[][{len(mul[0])}] {to_string(mul)};')
print(f'int candidatesDiv[] {to_string(div)};')
