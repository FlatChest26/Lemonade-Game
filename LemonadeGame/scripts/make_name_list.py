current_dir = "C:\\Users\\Romeo\\source\\repos\\LemonadeGameProject\\LemonadeGame\\scripts\\"

with open(current_dir + "last-names.txt", "r", encoding="utf-8") as read_file :
	with open(current_dir + "last-names_name_set.txt", "w", encoding="utf-8") as write_file:
		for line in read_file:
			write_file.write(f"\"{line.strip().lower().capitalize()}\",\n")