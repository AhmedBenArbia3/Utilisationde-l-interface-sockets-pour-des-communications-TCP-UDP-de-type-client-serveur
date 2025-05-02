# 		TP Client/Serveur – GL2 : Utilisation de l’interface 		sockets pour des communications TCP/UDP de type

# 						client/serveur



### Manipulation 2.1 (Vérification de la présence du serveur HTTP)

- Pour Contacter le serveur HTTP sur le port 8080 (le port 80 standard de HTTP est utilisé) de la machine serveur (127.0.0.1) et vérifier que le serveur est en bon état de marche, on utilise la commande: `telnet 127.0.0.1 8080`

- La requête `HEAD` ne retourne que les **en-têtes HTTP**

  La requête `GET`, elle, retourne **les en-têtes ET le contenu de la page**

### Manipulation 2.2 (Client HTTP en mode connecté)

- La connexion TCP s'établit à l'aide du **handshake en trois étapes**

  `Client → Serveur : SYN
  Serveur → Client : SYN-ACK
  Client → Serveur : ACK`

- La fermeture peut être décidée par le **client ou le serveur**, selon le contexte :

  - Si le serveur a fini d’envoyer la réponse (HTTP/1.0 ou `Connection: close`), il peut fermer la connexion.
  - Le client peut aussi décider de fermer la connexion après avoir reçu la réponse.

  `Client → Serveur : FIN
  Serveur → Client : ACK
  Serveur → Client : FIN
  Client → Serveur : ACK`

- **Serveur** : le port **8080**

  **Client** : utilise un **port éphémère** **60886**

`Temps →
Client                           Serveur
  | ------ SYN ----------------> |
  | <----- SYN-ACK ------------- |
  | ------ ACK ----------------> |
  |                              |  ← Connexion TCP établie
  | ------ HTTP GET -----------> |
  | <----- HTTP 200 OK -------- |
  |                              |
  | ------ FIN ----------------> |  ← Client initie la fermeture
  | <----- ACK ----------------- |
  | <----- FIN ----------------- |
  | ------ ACK ----------------> |`

- Si au lieu du `http_client` , on utilise `telnet` , on observant la capture, , on remarque que la requête HTTP est envoyée par TCP et non par HTTP , le contenu de la requête s'envoie dans le datagramme.

  ### Manipulation 3: (Transfert de messages en mode connecté)

  **Avec et sans `sleep(1)` :**

  - Sans délai, les messages peuvent être envoyés très rapidement et "batchés" ensemble côté TCP. Le client pourrait alors les lire tous d’un coup.
  - Avec `sleep(1)`, chaque message est espacé. Cela rend les messages plus lisibles et répartis temporellement.
  - **Constat :** En TCP, **il n'y a normalement pas de pertes de messages**, mais plusieurs messages peuvent être regroupés dans un segment TCP.

  **Avec Wireshark :**

  - Les **segments TCP** ne correspondent pas exactement à chaque `send()` ; plusieurs envois peuvent être **coalescés** (Nagle’s algorithm).
  - Wireshark montre que le protocole TCP ne garantit pas la **frontière des messages** (contrairement à UDP).

  **Débranchement du câble réseau :**

  - Si le câble est **débranché**, la socket TCP reste ouverte un certain temps (le timeout TCP).
  - Si on **rebranche rapidement**, la connexion peut survivre. Sinon, la socket finit par se fermer (RST ou timeout).
  - Le client ou le serveur peuvent bloquer ou recevoir une erreur à la prochaine opération (`read`, `send`).

  **Connexion multiple :**

  - Actuellement, le serveur ne gère **qu'un seul client** à la fois (`accept()` puis traitement bloquant).

  - Pour gérer plusieurs clients simultanément :

    - Utiliser `fork()`, `pthread`, ou `select()/poll()`.

  - Si la **file d’attente de `listen()`** (définie ici à `5`) est pleine :

    - Les nouvelles connexions sont **rejetées** .

    

    ### Manipulation 4: (Transfert de messages en mode non connecté)

| Critère          | TCP (connecté)                           | UDP (non connecté)                      |
| ---------------- | ---------------------------------------- | --------------------------------------- |
| Fiabilité        | Messages fiables, ordre garanti          | Pas de garantie de livraison ou d’ordre |
| Latence          | Plus élevée (établissement de connexion) | Plus faible                             |
| Perte de paquets | Rare, sauf coupure réseau                | Possible même sans panne                |

- **Plusieurs clients simultanés**

  + UDP **supporte naturellement plusieurs clients**.

  + Chaque `recvfrom()` indique **l’adresse de l’expéditeur** → le serveur peut répondre à chacun individuellement,.

  ### Manipulation 5: Serveur en mode concurrent

  1) #### **Test pour démontrer le parallélisme :**

     1. Lancez le **serveur**.
     2. Ouvrez plusieurs **terminaux clients** et connectez-vous à l’aide de `telnet localhost 12345`.
     3. Tapez quelques messages dans chaque terminal.
     4. Observez que les messages de chaque client sont traités **en parallèle** (simultanément, avec PID différents).
     5. Vous pouvez mesurer l'écoulement du temps total avec `time` en lançant plusieurs clients — vous constaterez que le temps **total n'augmente pas proportionnellement au nombre de clients**, preuve du parallélisme.

  2) ### **Test pour le multi-services**

     1. Lancez le serveur multi-services.
     2. Utilisez 3 petits programmes clients différents ou `telnet` et tapez `1`, `2` ou `3`.
     3. Lancez **plusieurs clients pour des services différents en même temps**.
     4. Observez que tous les services répondent **simultanément** (fork + switch).
     5. Utilisez `time ./client` ou `ps` pour observer les PIDs actifs.

     ------