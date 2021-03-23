/*
run 'npm install' to get dependencies.
'npm run dev'
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
      urls: ["stun:13.74.199.118:3478"],
    },
  ],
};

let peerConnection = new RTCPeerConnection(config);
let dataChannel = peerConnection.createDataChannel('datachannel');
let localStream = null;
let remoteStream = null;
let displayMediaStream = null;
let senders = [];

// HTML elements
const webcamButton = document.getElementById("webcamButton");
const webcamVideo = document.getElementById("webcamVideo");
const callButton = document.getElementById("callButton");
const invitationCode = document.getElementById("invitationCode");
const answerButton = document.getElementById("answerButton");
const remoteVideo = document.getElementById("remoteVideo");
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
const sendMessageButton = document.getElementById("sendMessageButton");
let dataChannelSend = document.getElementById("dataChannelSend");
let messageBox = document.getElementById("messageBox");
const backButton = document.getElementById("backButton");
const backButton2 = document.getElementById("backButton2");


backButton.addEventListener("click", () => {
  goBack();
})
backButton2.addEventListener("click", () => {
  goBack();
})

let goBack = () => {
  if(modalContent3.style.display == "block"){
    modalContent3.style.display = "none";
  }
  if(modalContent4.style.display == "block"){
    modalContent4.style.display = "none";
  }
  modalContent2.style.display = "block";
}

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

peerConnection.addEventListener('datachannel', event => {
  dataChannel = event.channel;
});

sendMessageButton.addEventListener('click', event => {
  const message = dataChannelSend.value;
  dataChannel.send(message);
  messageBox.value += 'Me: ' + message + '\n';
  dataChannelSend.value = '';
});

dataChannel.addEventListener('message', event => {
  const message = event.data;
  messageBox.value += message + '\n';
})


// Setting up media sources
webcamButton.onclick = async () => {
  localStream = await navigator.mediaDevices.getUserMedia({
    video: true,
    audio: false,
  });
  remoteStream = new MediaStream();

  localStream.getTracks().forEach((track) => {
    senders.push(peerConnection.addTrack(track, localStream));
  });

  peerConnection.ontrack = (event) => {
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
});

// Start a call and make offer
callButton.onclick = async () => {
  // Reference Firestore collections for signaling
  const callDoc = firestore.collection("calls").doc();
  const offerCandidates = callDoc.collection("offerCandidates");
  const answerCandidates = callDoc.collection("answerCandidates");

  invitationCode.value = callDoc.id;

  peerConnection.onicecandidate = (event) => {
    event.candidate && offerCandidates.add(event.candidate.toJSON());
  };
  
  // Create offer
  const offerDescription = await peerConnection.createOffer();
  await peerConnection.setLocalDescription(offerDescription);
  
  const offer = {
    sdp: offerDescription.sdp,
    type: offerDescription.type,
  };
  
  await callDoc.set({ offer });

  callDoc.onSnapshot((snapshot) => {
    const data = snapshot.data();
    if (!peerConnection.currentRemoteDescription && data?.answer) {
      const answerDescription = new RTCSessionDescription(data.answer);
      peerConnection.setRemoteDescription(answerDescription);
      modal.style.display = "none"
    }
  });

  answerCandidates.onSnapshot((snapshot) => {
    snapshot.docChanges().forEach((change) => {
      if (change.type === "added") {
        const candidate = new RTCIceCandidate(change.doc.data());
        peerConnection.addIceCandidate(candidate);
      }
    });
  });
};

// Answer call
answerButton.onclick = async () => {
  const callId = callInput.value;
  const callDoc = firestore.collection("calls").doc(callId);
  const answerCandidates = callDoc.collection("answerCandidates");
  const offerCandidates = callDoc.collection("offerCandidates");

  peerConnection.onicecandidate = (event) => {
    event.candidate && answerCandidates.add(event.candidate.toJSON());
  };
  
  const callData = (await callDoc.get()).data();

  const offerDescription = callData.offer;
  await peerConnection.setRemoteDescription(new RTCSessionDescription(offerDescription));

  const answerDescription = await peerConnection.createAnswer();
  await peerConnection.setLocalDescription(answerDescription);

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
        peerConnection.addIceCandidate(new RTCIceCandidate(data));
      }
    });
  });
};
