/*
run 'npm install' to get dependencies.
To run dev server, make sure to be in the webrtc-application folder, and run 'npm run dev'.
*/


import "./style.css";
import firebase from "firebase/app";
import "firebase/firestore";

const firebaseConfig = {
  apiKey: "AIzaSyDzYow_vM7DhBFlq9JyAIvzLcvYkb5X9qE",
  authDomain: "fir-rtc-523e4.firebaseapp.com",
  projectId: "fir-rtc-523e4",
  storageBucket: "fir-rtc-523e4.appspot.com",
  messagingSenderId: "464743646271",
  appId: "1:464743646271:web:629b63cfdddc3b6d0ea48a",
  measurementId: "G-WJQNJMQ1VG",
};

if (!firebase.apps.length) {
  firebase.initializeApp(firebaseConfig);
}
const firestore = firebase.firestore();

const config = {
  iceServers: [
    {
      urls: ["stun:stun1.l.google.com:19302"],
    },
  ],
};

// Global State
const pc = new RTCPeerConnection(config);
let localStream = null;
let remoteStream = null;
let displayMediaStream = null;

let senders = [];

// HTML elements
const webcamButton = document.getElementById("webcamButton");
const webcamVideo = document.getElementById("webcamVideo");
const callButton = document.getElementById("callButton");
//const callInput = document.getElementById("callInput");
const invitationCode = document.getElementById("invitationCode");
const answerButton = document.getElementById("answerButton");
const remoteVideo = document.getElementById("remoteVideo");
const hangupButton = document.getElementById("hangupButton");
const dropdownButton = document.getElementById("dropdownButton");
const refreshButton = document.getElementById('refreshButton');
const shareButton = document.getElementById("share-button");
const stopShareButton = document.getElementById("stop-share-button");
const joinButton = document.getElementById("joinButton");
const hostButton = document.getElementById("hostButton");
const modal = document.getElementById("myModal");
const modalContent1 = document.getElementById("modalCnt1");
const modalContent2 = document.getElementById("modalCnt2");
const modalContent3 = document.getElementById("modalCnt3");
const modalContent4 = document.getElementById("modalCnt4");

joinButton.addEventListener("click", () => {
  console.log("join button clicked");
  modalContent2.style.display = "none";
  modalContent3.style.display= "block";
});

hostButton.addEventListener("click", () => {
  console.log("host button clicked");
  modalContent2.style.display = "none";
  modalContent4.style.display = "block"
});



// 1. Setup media sources

webcamButton.onclick = async () => {
  localStream = await navigator.mediaDevices.getUserMedia({
    video: true,
    audio: false,
  });
  remoteStream = new MediaStream();


  // Push tracks from local stream to peer connection
  localStream.getTracks().forEach((track) => {
    senders.push(pc.addTrack(track, localStream));
  });

  // Pull tracks from remote stream, add to video stream
  pc.ontrack = (event) => {
    event.streams[0].getTracks().forEach((track) => {
      remoteStream.addTrack(track);
    });
  };

  webcamVideo.srcObject = localStream;
  remoteVideo.srcObject = remoteStream;

  if(localStream != null){
    modalContent1.style.display = "none";
    modalContent2.style.display = "block";
  }
};

shareButton.addEventListener("click", async () => {
  if(!displayMediaStream){
    displayMediaStream = await navigator.mediaDevices.getDisplayMedia();
  }

  senders.find(sender => sender.track.kind == 'video').replaceTrack(displayMediaStream.getTracks()[0]);
  
  webcamVideo.srcObject = displayMediaStream;

  shareButton.style.display = "none";
  stopShareButton.style.display = "inline";
});

stopShareButton.addEventListener("click", async (event) => {
  senders.find(sender => sender.track.kind == 'video').replaceTrack(localStream.getTracks().find(track => track.kind === 'video'));

  webcamVideo.srcObject = localStream;
  shareButton.style.display = "inline";
  stopShareButton.style.display = 'none';
})

// 2. Create an offer
callButton.onclick = async () => {
  // Reference Firestore collections for signaling
  const callDoc = firestore.collection("calls").doc();
  const offerCandidates = callDoc.collection("offerCandidates");
  const answerCandidates = callDoc.collection("answerCandidates");

  invitationCode.value = callDoc.id;

  // Get candidates for caller, save to db
  pc.onicecandidate = (event) => {
    event.candidate && offerCandidates.add(event.candidate.toJSON());
  };

  // Create offer
  const offerDescription = await pc.createOffer();
  await pc.setLocalDescription(offerDescription);

  const offer = {
    sdp: offerDescription.sdp,
    type: offerDescription.type,
  };

  await callDoc.set({ offer });

  // Listen for remote answer
  callDoc.onSnapshot((snapshot) => {
    const data = snapshot.data();
    if (!pc.currentRemoteDescription && data?.answer) {
      const answerDescription = new RTCSessionDescription(data.answer);
      pc.setRemoteDescription(answerDescription);
      modal.style.display = "none"
    }
  });

  // When answered, add candidate to peer connection
  answerCandidates.onSnapshot((snapshot) => {
    snapshot.docChanges().forEach((change) => {
      if (change.type === "added") {
        const candidate = new RTCIceCandidate(change.doc.data());
        pc.addIceCandidate(candidate);
      }
    });
  });

};

// Refresh available calls
refreshButton.onclick = async () => {
  const callsRef = firestore.collection('calls');
  const snapshot = await callsRef.get();
  if (snapshot.empty) {
    console.log('No matching calls');
    return;
  }

  snapshot.forEach(doc => {
    console.log(doc.id, '=>', doc.data().offer);
  })
}

// 3. Answer the call with the unique ID
answerButton.onclick = async () => {
  const callId = callInput.value;
  const callDoc = firestore.collection("calls").doc(callId);
  const answerCandidates = callDoc.collection("answerCandidates");
  const offerCandidates = callDoc.collection("offerCandidates");

  pc.onicecandidate = (event) => {
    event.candidate && answerCandidates.add(event.candidate.toJSON());
  };

  const callData = (await callDoc.get()).data();

  const offerDescription = callData.offer;
  await pc.setRemoteDescription(new RTCSessionDescription(offerDescription));

  const answerDescription = await pc.createAnswer();
  await pc.setLocalDescription(answerDescription);

  const answer = {
    type: answerDescription.type,
    sdp: answerDescription.sdp,
  };

  await callDoc.update({ answer });

  offerCandidates.onSnapshot((snapshot) => {
    snapshot.docChanges().forEach((change) => {
      console.log(change);
      if (change.type === "added") {
        modal.style.display = "none";
        let data = change.doc.data();
        pc.addIceCandidate(new RTCIceCandidate(data));
      }
    });
  });
};
