pipeline {
  agent any
  stages {
    stage('Setup dependencies') {
      steps {
        withCredentials([string(credentialsId: 'github-token', variable: 'GITHUB_OAUTH_TOKEN')]) {
          sh '''#!/bin/bash
            wget https://github.com/gruntwork-io/fetch/releases/download/v0.1.1/fetch_linux_amd64
            chmod +x fetch_linux_amd64
            source version.sh
            ./fetch_linux_amd64 --repo="https://github.com/RIAPS/riaps-externals/" --tag=$externalsversion --release-asset="riaps-externals-amd64.deb" .
            sudo dpkg -i riaps-externals-amd64.deb
          '''
        }
      }
    }
    stage('Build') {
      steps {
        sh '''#!/bin/bash
          chmod +x build.sh
          ./build.sh
        '''
      }
    }
    stage('Package') {
      steps {
        sh '''#!/bin/bash
          chmod +x package.sh
          ./package.sh
          mv package/riaps-core-amd64.deb .
          mv package/riaps-core-armhf.deb .
        '''
      }
    }
    stage('Deploy') {
      when { buildingTag() }
      steps {
        // Install github-release cli tool to build directory
        sh 'GOPATH=$WORKSPACE/go go get github.com/aktau/github-release'
        // Use GitHub OAuth token stored in 'github-token' credentials
        withCredentials([string(credentialsId: 'github-token', variable: 'GITHUB_TOKEN')]) {
          script {
            def user = 'riaps'
            def repo = 'riaps-core'
            def files = ['riaps-core-amd64.deb','riaps-core-armhf.deb']
            // Create release on GitHub, if it doesn't already exist
            sh "${env.WORKSPACE}/go/bin/github-release release --user ${user} --repo ${repo} --tag ${env.TAG_NAME} --name ${env.TAG_NAME} --pre-release || true"
            // Iterate over artifacts and upload them
            for(int i = 0; i < files.size(); i++){
              sh "${env.WORKSPACE}/go/bin/github-release upload -R --user ${user} --repo ${repo} --tag ${env.TAG_NAME} --name ${files[i]} --file ${files[i]}" 
            } 
          }
        }
      }
    }
  }
  post {
    success {
      archiveArtifacts artifacts: '*.deb', fingerprint: true
    }
  }
}
