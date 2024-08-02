#!/usr/bin/env python3

import subprocess

def ssh_successful(host):
    try:
        print(f"Evaluando maquina {host}")
        result = subprocess.run(["nc", "-zv", host, "22"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode != 0:
            return False
        result = subprocess.run(
            ["ssh", "-o", "ConnectTimeout=2", "-o", "BatchMode=yes", "-o", "StrictHostKeyChecking=no", host, "exit"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )
        return result.returncode == 0
    except subprocess.CalledProcessError:
        return False

def main():
    maquinas = []
    pcunix = 'pcunix'

    for i in range(100, 144):
       nombre_maquina = pcunix + str(i)
       maquinas.append(nombre_maquina)

    with open("mis_hosts.txt", "w") as file:
        for host in maquinas:
            if ssh_successful(host):
                print(f"Conectado a {host}")
                file.write(host + "\n")
                file.flush()

if __name__ == "__main__":
    main()
