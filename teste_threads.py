import random, multiprocessing

def gen_test(file, heaps, operations, lower, upper):
    '''
    prima linie: numar de multimi, urmat de operatii

    operatii:
    1 : inserati numarul X in multime
    2 : afisati cel mai mic numar
    3 : stergeti cel mai mic numar din multime (daca acesta exista)
    4 : reuniti multimile A si B ( multimea A preia elementele mulţimii B, care rămâne vidă )
    '''
    with open(file, "w") as f:
        print(heaps, operations, file=f)
        numbers = set()

        for _ in range(operations):
            operation = random.randint(1, 4)

            match operation:
                case 1:
                    number = random.randint(lower, upper)
                    heap = random.randint(0, heaps - 1)
                    print(operation, number, heap, file=f)
                case 2 | 3:
                    heap = random.randint(0, heaps - 1)
                    print(operation, heap, file=f)

                case 4:
                    a = random.randint(0, heaps - 1)
                    b = random.randint(0, heaps - 1)
                    print(operation, a, b, file=f)


def main():
    nr_heaps = 10
    nr_ops = 20_000_000

    jobs = [
        ("1k.in", nr_heaps, nr_ops, -1000, 1000),
        ("1m.in", nr_heaps, nr_ops, -1_000_000, 1_000_000),
        ("1b.in", nr_heaps, nr_ops, -1_000_000_000, 1_000_000_000),
        ("0-1.in", nr_heaps, nr_ops, 0, 1),
        ("ll.in", nr_heaps, nr_ops, -2**63, 2**63),
        ("ull.in", nr_heaps, nr_ops, 0, 2**64),
    ]

    with multiprocessing.Pool(processes=len(jobs)) as pool:
        pool.starmap(gen_test, jobs)

if __name__ == "__main__":
    multiprocessing.set_start_method("fork")  # "spawn" on Windows
    main()
