# P2P-app-with-STUN

## WebRTC
WebRTC is used to achieve real-time communication between peers. The functionalities implemented in this application are video, voice, screen share and generic data sharing as a basic chat function. Inspiration has been gathered through the course on webrtc.org.

### Implemented functionality
We have used Firebase as a signaling server to let peers be able to connect to eachother, alongside our own made STUN-server, which you can read about [here](https://github.com/mattiaseggen/P2P-app-with-STUN/blob/main/STUN-server/README.md)(in the README.md in STUN-server folder).
To host the website we used Netlify, which is also used for continous deployment. Everytime a commit is pushed to main, Netlify updates our website.

### How to run
Make sure current directory is webrtc-application
```
npm install
npm run dev
```
Optionally, use link provided by Netlify 
