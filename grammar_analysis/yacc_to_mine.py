import re

def main(fname):
    with open(fname) as f:
        text = f.read()

    # nons = re.findall("([a-zA-Z].+)", text)
    # print(nons)
    # non_ter = "gen.set_non_terminars(\"{}\");\n".format(' '.join(nons))
    
    text = re.sub(r"%token\s(.+)", "\"\\1 \"", text)
    text = re.sub(r"((?:.|\n)*)%start", "Generators gen;\ngen.set_terminators(\\1);\n{}\n%start".format(""), text)
    text = re.sub(r"%start\s(.+)", "gen.set_start(\"\\1\");\n", text)

    

    fname = fname.replace("yacc", "mine")
    with open(fname, "w") as f:
        f.write(text)


if __name__ == '__main__':
    main("grammer_yacc.txt")