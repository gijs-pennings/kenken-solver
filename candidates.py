import functools
import itertools

MAX_CAGE = 5
MAX_VALUE = 9

values = range(1, MAX_VALUE + 1)
pairs = list(itertools.product(values, values))

def find(mat, op):
    for size in range(len(mat), MAX_CAGE + 1):
        lst = []
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
                    if x >= len(lst): lst.extend([0] * (x - len(lst) + 1))
                    lst[x] |= to_mask(b)
                else:
                    stack.append(b)
        mat.append(lst)

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
    return '{ ' + ', '.join(items) + ' }' if len(items) > 0 else '{}'

add = [[]]    # [size][target]
sub = [0]     # [target] since always size == 2
mul = [[]]    # like add
div = [0, 0]  # like sub

find(add, sum)
find(mul, lambda lst: functools.reduce(lambda x,y: x*y, lst))
find_size2(sub, lambda x,y: abs(x-y))
find_size2(div, lambda x,y: max(x,y) / min(x,y))

sub.pop() # always zero

print(f'const short add[][{len(add[-1])}] {to_string(add)};')
print(f'const short sub[] {to_string(sub)};')
print(f'const short mul[][{len(mul[-1])}] {to_string(mul)};')
print(f'const short div[] {to_string(div)};')
