import serial
import time
import tkinter as tk

# ------------------------------ FONCTIONS ----------------------------------------
def fermer():
    root.destroy()
    ser.close()

def afficher_frame(frame):
    for f in [frame_menu, frame_parametres, frame_pilote_auto]:
        f.pack_forget()
    frame.pack(expand=True, fill="both")
    
# Envoi de la consigne (trace = 1)
def f_send_cap_consigne():
    try:
        valeur = float(entry_consigne.get())
        if 0 <= valeur <= 360:
            envoyer_variable(1, valeur)
            entry_consigne.delete(0,tk.END)
        else:
            label_valeur_consigne.config(text="Valeur invalide")
            entry_consigne.delete(0,tk.END)
    except ValueError:
        label_valeur_consigne.config(text="Entrée non valide")
        entry_consigne.delete(0,tk.END)

# Mode automatique (trace = 2)
def f_auto():
    envoyer_variable(2, 0)
    label_mode_auto_manu.config(text="Mode Automatique activé")

# Mode manuel (trace = 3)
def f_manu():
    envoyer_variable(3, 0)
    label_mode_auto_manu.config(text="Mode Manuel activé") 
#def f_changer_kp (trace = 4)
def f_Kp():
    try:
        valeur = float(entry_kp.get())
        if valeur >= 0 and valeur<= 100 :
            envoyer_variable(4,valeur)
            entry_kp.delete(0,tk.END)
        else :
            label_affichage_kp.config(text="valeur hors plage")
            entry_kp.delete(0,tk.END)
    except ValueError:
        label_affichage_kp.config(text="Entrée non valide")
        entry_kp.delete(0,tk.END)


# def f_changer_Ti (trace = 5 )
def f_Ti():
    try:
        valeur = float(entry_ti.get())
        if valeur >= 0.00 and valeur <= 5.00 :
            envoyer_variable(5,valeur)
            entry_ti.delete(0,tk.END)
        else :
            label_affichage_ti.config(text="valeur hors plage")
            entry_ti.delete(0,tk.END)
    except ValueError:
        label_affichage_ti.config(text="Entrée non valide")
        entry_ti.delete(0,tk.END)
        
        
        
        
# def f_changer_alpha (trace = 6)  rapport cyclique
def f_changer_alpha():
    try:
        valeur = float(entry_alpha.get())
        if  valeur >= 0 and valeur <= 100:
            envoyer_variable(6,valeur)
            entry_alpha.delete(0,tk.END)
        else:
            label_affichage_alpha.config(text="valeur hors plage")
            entry_alpha.delete(0,tk.END)
    except ValueError:
        label_affichage_alpha.config(text="Entrée non valide")
        entry_alpha.delete(0,tk.END)
        
        
        
        
# Envoi d'une variable vers l'Arduino
def envoyer_variable(identifiant, valeur):
    data = identifiant.to_bytes(1, 'little')
    valeur_int = int(valeur * 100)  # conserver 2 décimales
    data += valeur_int.to_bytes(2, 'little', signed=False)
    ser.write(data)
    time.sleep(0.2)


# ------------------Lecture série sans bloquer l'IHM------------------------
def lecture_serie():
    if ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line:
                cap_actuel_str, cap_consigne_str ,kp_str,ti_str,alpha_pourcent_str= line.split(',')
                cap_actuel = float(cap_actuel_str)
                cap_consigne = float(cap_consigne_str)
                kp =float(kp_str)
                ti=float(ti_str)
                alpha_pourcent=float(alpha_pourcent_str)
                label_valeur_consigne.config(text=f"{cap_consigne} °")
                label_valeur_cap_actuel.config(text=f"{cap_actuel} °")
                label_affichage_kp.config(text=f"{kp} %/°")
                label_affichage_ti.config(text=f"{ti}s")
                label_affichage_alpha.config(text=f"{alpha_pourcent}%")
                
        except ValueError:
            pass  # ligne mal formée ignorée
    root.after(100, lecture_serie)

# ----------------------------- INTERFACE TKINTER ---------------------------------------

root = tk.Tk()
root.title("IHM ROV")
root.geometry("1920x1040")
root.resizable(False, False)

# ---------------------- Frame Menu Principal ----------------------
frame_menu = tk.Frame(root)

label_menu = tk.Label(frame_menu, text="Menu IHM", font=("Courier", 48, "bold"))
label_menu.pack(pady=100)

frame_boutons_menu = tk.Frame(frame_menu)
frame_boutons_menu.pack(pady=50)

bouton_pilote_auto = tk.Button(frame_boutons_menu, text="Pilote Automatique", bg="#b2daf9", font=("Courier", 40), width=30,
                               command=lambda: afficher_frame(frame_pilote_auto))
bouton_pilote_auto.pack(pady=20)

bouton_parametres = tk.Button(frame_boutons_menu, text="Paramètres", bg="#7df87b", font=("Courier", 40), width=30,
                              command=lambda: afficher_frame(frame_parametres))
bouton_parametres.pack(pady=20)

# ---------------------- Frame Paramètres ----------------------
frame_parametres = tk.Frame(root, bg="#7df87b")

label_parametres = tk.Label(frame_parametres, text="Menu Paramètres", bg="#7df87b", font=("Courier", 40, "bold"))
label_parametres.pack(pady=60)

bouton_retour_param = tk.Button(frame_parametres, text="Retour", command=lambda: afficher_frame(frame_menu),
                                bg='green', font=("Courier", 40))
bouton_retour_param.pack(side="bottom", pady=30)


# --- Section Kp ---
frame_kp = tk.Frame(frame_parametres, bg="#7df87b")
frame_kp.pack(pady=20)

label_kp = tk.Label(frame_kp, text="Valeur Kp :", bg="#7df87b", font=("Courier", 20, "bold"))
label_kp.pack(side="left", padx=10)

entry_kp = tk.Entry(frame_kp, font=("Courier", 25), width=20)
entry_kp.pack(side="left", padx=10)

label_affichage_kp =tk.Label(frame_kp , text="En attente", bg="#7df87b", font=("Courier", 20, "bold"))
label_affichage_kp.pack(side="left", padx=10)


btn_valider_kp= tk.Button(frame_kp, command= f_Kp, text="Valider", font=("Courier", 20), bg='grey')
btn_valider_kp.pack(side = "left", pady=10)

# --- Section Ti ---
frame_ti = tk.Frame(frame_parametres, bg="#7df87b")
frame_ti.pack(pady=20)

label_ti = tk.Label(frame_ti, text="Valeur Ti :", bg="#7df87b", font=("Courier", 20, "bold"))
label_ti.pack(side="left", padx=10)

entry_ti = tk.Entry(frame_ti, font=("Courier", 25), width=20)
entry_ti.pack(side="left", padx=10)

label_affichage_ti =tk.Label(frame_ti, text="En attente", bg="#7df87b", font=("Courier", 20, "bold"))
label_affichage_ti.pack(side="left", padx=10)

btn_valider_Ti= tk.Button(frame_ti, command= f_Ti, text="Valider", font=("Courier", 20), bg='grey')
btn_valider_Ti.pack(side = "left", pady=10)

# --- Rapport cyclique alpha(%) ---
frame_alpha = tk.Frame(frame_parametres, bg="#7df87b")
frame_alpha.pack(pady=20)

label_alpha = tk.Label(frame_alpha, text="Valeur rapport cyclique alpha (%) :", bg="#7df87b", font=("Courier", 20, "bold"))
label_alpha.pack(side="left", padx=10)

entry_alpha = tk.Entry(frame_alpha, font=("Courier", 25), width=10)
entry_alpha.pack(side="left", padx=10)

label_affichage_alpha =tk.Label(frame_alpha, text="En attente", bg="#7df87b", font=("Courier", 20, "bold"))
label_affichage_alpha.pack(side="left", padx=10)

btn_valider_alpha= tk.Button(frame_alpha, command= f_changer_alpha, text="Valider", font=("Courier", 20), bg='grey')
btn_valider_alpha.pack(side = "left", pady=10)
# ---------------------- Frame Pilote Automatique ----------------------
frame_pilote_auto = tk.Frame(root, bg="#b2daf9")

label_pilote = tk.Label(frame_pilote_auto, text="Menu Pilote Automatique", bg="#b2daf9", font=("Courier", 40, "bold"))
label_pilote.pack(pady=20)

# -------- Section Consigne --------
frame_consigne = tk.Frame(frame_pilote_auto, bg="#b2daf9")
frame_consigne.pack(pady=10)

label_consigne = tk.Label(frame_consigne, text="Cap Consigne (0 - 360)", bg="#b2daf9", font=("Courier", 25, "bold"))
label_consigne.pack(pady=10)

label_valeur_consigne = tk.Label(frame_consigne, text="0 °", bg="#b2daf9", font=("Courier", 20))
label_valeur_consigne.pack(pady=10)

entry_consigne = tk.Entry(frame_consigne, font=("Courier", 20), width=20)
entry_consigne.pack()

btn_valider_consigne= tk.Button(frame_consigne, command=f_send_cap_consigne, text="Valider", font=("Courier", 30), bg='grey')
btn_valider_consigne.pack(pady=10)

# -------- Section Cap actuel --------
label_cap_actuel = tk.Label(frame_pilote_auto, text="Cap Actuel", bg="#b2daf9", font=("Courier", 25, "bold"))
label_cap_actuel.pack(pady=20)

label_valeur_cap_actuel = tk.Label(frame_pilote_auto, text="En attente", bg="#b2daf9", font=("Courier", 20))
label_valeur_cap_actuel.pack(pady=10)

# -------- Section Mode Manuel / Auto --------
label_mode = tk.Label(frame_pilote_auto, text="Mode Manuel / Automatique", bg="#b2daf9", font=("Courier", 25, "bold"))
label_mode.pack(pady=10)

frame_modes = tk.Frame(frame_pilote_auto, bg="#b2daf9")
frame_modes.pack(pady=10)

btn_manuel = tk.Button(frame_modes, text="Manuel", command=f_manu, font=("Courier", 30), width=15, bg='grey')
btn_manuel.grid(row=0, column=0, padx=10)

btn_auto = tk.Button(frame_modes, text="Auto", command=f_auto, font=("Courier", 30), width=15, bg='grey')
btn_auto.grid(row=0, column=1, padx=10)

label_mode_auto_manu = tk.Label(frame_pilote_auto, text="Mode Manuel activé", bg="#b2daf9", font=("Courier", 20))
label_mode_auto_manu.pack(pady=10)



btn_retour_auto = tk.Button(frame_pilote_auto, text="Retour", command=lambda: afficher_frame(frame_menu),bg='green', font=("Courier", 40))
btn_retour_auto.pack(side="bottom", pady=10)

# ---------------------- Bouton Fermer commun ----------------------
bouton_fermer = tk.Button(root, text="Fermer", command=fermer, bg='red', font=("Courier", 40))
bouton_fermer.place(relx=1.0, rely=1.0, anchor="se", x=-10, y=-10)

# Affichage initial
afficher_frame(frame_menu)

# Initialisation port série
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
time.sleep(2)  # Attente du reboot Arduino

# Lancer la lecture série périodique
lecture_serie()

# Boucle principale
root.mainloop()
