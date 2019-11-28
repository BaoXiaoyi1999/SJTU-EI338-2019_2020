class MemoryAllocator:
    def __init__(self, capacity):
        self.capacity = capacity
        self.storage = {}

    def request(self, name, space, strategy):
        holes = self._find_holes()  # holes are already sorted by index
        if strategy == "F":
            error = self._first_fit(name, space, holes)
        elif strategy == "B":
            error = self._best_fit(name, space, holes)
        elif strategy == "W":
            error = self._worst_fit(name, space, holes)
        else:
            print("Invalid strategy")
            error = 1
        return error  

    def release(self, name):
        if name in self.storage.keys():
            del self.storage[name]
            error = 0
        else:
            print("The process is not in memory.")
            error = 1
        return error

    def compact(self):
        allocated = list([(value, key) for (key, value) in self.storage.items()])
        allocated.sort()
        counter = 0
        for i in range(len(allocated)):
            (start, end), name = allocated[i]
            temp_space = end - start + 1
            start = counter
            end = start + temp_space - 1
            counter += temp_space
            allocated[i] = [(start, end), name]
        self.storage.clear()
        for (start, end), name in allocated:
            self.storage[name] = (start, end)


    def display(self):
        allocated = list([(value, key) for (key, value) in self.storage.items()])
        allocated.sort()
        status_list = []
        left_bound = 0
        for i in range(len(allocated)):
            right_bound = allocated[i][0][0] - 1
            if left_bound <= right_bound:
                status_list.append([(left_bound, right_bound), "Unused"])  # previous unused
            status_list.append([allocated[i][0], "Process {0}".format(allocated[i][1])])
            left_bound = allocated[i][0][1] + 1
        right_bound = self.capacity - 1
        if left_bound <= right_bound:
            status_list.append([(left_bound, right_bound), "Unused"])
        for status in status_list:
            print("Addresses [{0}:{1}] {2}".format(status[0][0], status[0][1], status[1]))

    def _first_fit(self, name, space, holes):
        error = 1
        for (start, end) in holes:
            available_space = end - start + 1
            if available_space > space:
                value = (start, start + space - 1)
                self.storage[name] = value
                error = 0
                break
        return error

    def _best_fit(self, name, space, holes):
        holes_length = [(hole[1] - hole[0] + 1, hole[0], hole[1]) for hole in holes]
        holes_length.sort()
        res = self._first_available(space, holes_length)
        if res == -1:
            error = 1
        else:
            start = holes_length[res][1]
            end = holes_length[res][1] + space - 1
            value = (start, end)
            self.storage[name] = value
            error = 0
        return error

    def _worst_fit(self, name, space, holes):
        holes_length = [(hole[1] - hole[0], hole[0], hole[1]) for hole in holes]
        holes_length.sort()
        holes_length.reverse()
        if len(holes_length) == 0:
            error = 1
        elif holes_length[0][0] < space:
            error = 1
        else:
            start = holes_length[0][1]
            end = holes_length[0][1] + space - 1
            value = (start, end)
            self.storage[name] = value
            error = 0
        return error

    def _find_holes(self):
        # binary search ? may be wrong
        allocated = list(self.storage.values())
        allocated.sort()
        holes = []
        left_bound = 0
        for i in range(len(allocated)):
            right_bound = allocated[i][0] - 1
            if left_bound <= right_bound:
                holes.append((left_bound, right_bound))
            left_bound = allocated[i][1] + 1
        right_bound = self.capacity - 1
        if left_bound <= right_bound:
            holes.append((left_bound, right_bound))
        return holes

    def _first_available(self, space, holes_length):
        left = 0
        right = len(holes_length) - 1
        first_ge = -1
        while left < right:
            mid = (left + right) // 2
            if holes_length[mid][0] == space:
                first_ge = mid
                break
            elif holes_length[mid][0] < space:
                left = mid + 1
            else:
                right = mid
        if holes_length[left][0] >= space:
            first_ge = left
        return first_ge


def user_program():
    while True:
        command = input("allocator>").split()
        if command[0] == "RQ":
            process_name = command[1]
            process_space = int(command[2])
            allocate_strategy = command[3]
            error = m.request(process_name, process_space, allocate_strategy)
            if error:
                print("Request fail.")
            else:
                print("Request succeed.")
        elif command[0] == "RL":
            error = m.release(command[1])
            if error:
                print("Release fail.")
            else:
                print("Release succeed.")

        elif command[0] == "C":
            m.compact()
            print("Compact succeed.")

        elif command[0] == "STAT":
            m.display()
        elif command[0] == "X":
            print("Exit.")
            break
        else:
            print("Invalid command.Exit.")
            break


# space_capacity = int(input("Enter the initial amount of memory:"))

space_capacity = 128
m = MemoryAllocator(space_capacity)
user_program()
