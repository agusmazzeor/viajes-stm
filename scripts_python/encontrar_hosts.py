#!/usr/bin/env python3

import subprocess

def ssh_successful(host):
    try:
        print(f"Evaluando maquina {host}")
        # Check if SSH port (22) is open
        result = subprocess.run(["nc", "-zv", host, "22"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode != 0:
            return False
        # Try SSH connection with a timeout of 2 seconds, in BatchMode, and accept new host keys automatically
        result = subprocess.run(
            ["ssh", "-o", "ConnectTimeout=2", "-o", "BatchMode=yes", "-o", "StrictHostKeyChecking=no", host, "exit"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )
        return result.returncode == 0
    except subprocess.CalledProcessError:
        return False

def main():
    # Lista de maquinas con números entre 100 and 144
    maquinas = []
    pcunix = 'pcunix'

    for i in range(100, 144):
       nombre_maquina = pcunix + str(i)
       maquinas.append(nombre_maquina)

    # Abre el archivo en modo de escritura, lo que trunca el archivo a 0 bytes si ya existe
    with open("mis_hosts.txt", "w") as file:
        # Verifica cada host y lo escribe en el archivo si puedo hacer ssh
        for host in maquinas:
            if ssh_successful(host):
                print(f"Conectado a {host}")
                file.write(host + "\n")
                file.flush()  # Forzar escritura a disco

if __name__ == "__main__":
    main()
