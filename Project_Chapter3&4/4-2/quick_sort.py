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

    def run_function(self, arr, lb, ub):
        if self.is_sort_thread:
            quick_sort(arr, lb, ub)
            return 0
        else:
            pi = partition(arr, lb, ub)
            return pi


def quick_sort(arr, lb, ub, threshold=10):
    if lb < ub:
        if ub - lb < threshold:
            temp = arr[lb: ub+1]
            temp.sort()
            for i in range(lb, ub+1):
                arr[i] = temp[i-lb]
        else:
            print("partition thread starts:")
            thread_p = MyThread("partition_thread", "p")
            pi = thread_p.run_function(arr, lb, ub)
            thread_p.start()
            thread_p.join()
            print("end of partition thread.")
            print(arr)

            print("two sorting threads start:")
            thread_s1 = MyThread("sorting_thread1", "s")
            thread_s2 = MyThread("sorting_thread2", "s")
            thread_s1.run_function(arr, lb, pi-1)
            thread_s2.run_function(arr, pi+1, ub)

            thread_s1.start()
            thread_s2.start()
            thread_s1.join()
            thread_s2.join()
            print("end of sorting threads")
            print(arr)


def partition(arr, lb, ub):
    pivot = arr[ub]
    i = lb
    for j in range(lb, ub):
        if arr[j] < pivot:
            arr[i], arr[j] = arr[j], arr[i]
            i += 1
    arr[i], arr[ub] = arr[ub], arr[i]
    return i


int_arr = list(range(20))
random.shuffle(int_arr)
print("original array: ")
print(int_arr)
# int_arr = [7, 12, 19, 3, 18, 4, 2, 6, 15, 8, 11]
arr_size = len(int_arr)
quick_sort(int_arr, 0, arr_size-1)
