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
        '''
      }
    }
    stage('Archive artifacts') {
      steps {
        fileExists 'package/riaps-core-amd64.deb'
        archiveArtifacts(artifacts: 'package/riaps-core-amd64.deb', onlyIfSuccessful: true, fingerprint: true)
        fileExists 'package/riaps-core-armhf.deb'
        archiveArtifacts(artifacts: 'package/riaps-core-armhf.deb', onlyIfSuccessful: true, fingerprint: true)
      }
    }
  }
}
