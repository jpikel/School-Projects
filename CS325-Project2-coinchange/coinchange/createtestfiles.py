"""creates test files for question 3, 4, and 5"""

def createtestfile():
    """Creates test files to use for timing our algorithms"""
    file_name = "Sample3_40.txt"
    array_nums = [1, 5, 10, 25, 50] #array for sample 3
    #file_name = "Sample4V1_40.txt"
    #array_nums = [1, 2, 6, 12, 24, 48, 60] #array for sample4v1
    #file_name = "Sample4V2_40.txt"
    #array_nums = [1, 6, 13, 37, 150] #array for sample4v2
    #file_name = "Sample4V2high_n.txt"
    #array_nums = [1, 6, 13, 37, 150] #array for sample4V2high_n
    #file_name = "Sample4V1high_n.txt"
    #array_nums = [1, 2, 6, 12, 24, 48, 60] #array for sample4v1high_n
    #file_name = "Sample5_40.txt"
    #array_nums = [1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30]
    file_name = "./pow3/Power3.txt"
    array_nums = [1, 3, 9, 27]
    low = 2000
    high = 2200
    line = "["
    for item in array_nums:
        line += str(item) + ", "
    line = line[:-2]
    line += "]\n"
    file_handle = open(file_name, 'w')
    for i in range(low, high, 1):
        file_handle.write(line)
        line2 = str(i) + "\n"
        file_handle.write(line2)
    file_handle.close()

createtestfile()
