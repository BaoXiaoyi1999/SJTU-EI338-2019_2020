import threading
import random


class MyThread(threading.Thread):
    def __init__(self, name="thread", thread_type="s"):
        threading.Thread.__init__(self)
        self.name = name
        if thread_type == "s":
            self.is_sort_thread = True
        else:
            self.is_sort_thread = False

    def run_function(self, arr, lb, ub, mid=0):
        if self.is_sort_thread:
            merge_sort(arr, lb, ub)
            return 0
        else:
            merge(arr, lb, ub, mid)


def merge_sort(arr, lb, ub, threshold=10):
    if lb < ub:
        if ub - lb < threshold:
            temp = arr[lb: ub+1]
            temp.sort()
            for i in range(lb, ub+1):
                arr[i] = temp[i-lb]
        else:
            mid = (ub + lb) // 2
            print("two sorting threads start:")
            thread_s1 = MyThread("sort_thread1", "s")
            thread_s1.run_function(arr, lb, mid-1)
            thread_s2 = MyThread("sort_thread2", "s")
            thread_s2.run_function(arr, mid, ub)

            thread_s1.start()
            thread_s2.start()
            thread_s1.join()
            thread_s2.join()
            print("end of two sorting threads.")
            print(arr)

            print("merging thread start:")
            thread_m = MyThread("merging_thread", "m")
            thread_m.run_function(arr, lb, ub, mid)

            thread_m.start()
            thread_m.join()
            print("end of merging threads")
            print(arr)


def merge(arr, lb, ub, mid):
    # print(lb, ub, mid)
    new_arr = []
    i = lb
    j = mid
    while i < mid and j <= ub:
        if arr[i] < arr[j]:
            new_arr.append(arr[i])
            i += 1
        else:
            new_arr.append(arr[j])
            j += 1
    new_arr.extend(arr[i:mid])
    new_arr.extend(arr[j:ub+1])
    for k in range(lb, ub+1):
        arr[k] = new_arr[k-lb]


int_arr = list(range(20))
random.shuffle(int_arr)
print("original array: ")
print(int_arr)
# int_arr = [7, 12, 19, 3, 18, 4, 2, 6, 15, 8, 11]
arr_size = len(int_arr)
merge_sort(int_arr, 0, arr_size-1)
