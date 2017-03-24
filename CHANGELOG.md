**Dynamic v1.1.0.0**

* Inline with BTC 0.12
* HD Wallet Code Improvements
* Remove/Replace Boost usage for c++11
* Do not shadow member variables in httpserver
* Update dbwrapper_tests.cpp
* Access WorkQueue::running only within the cs lock
* Use BIP32_HARDENED_KEY_LIMIT
* Update NULL to use nullptr in GetWork & GetBlockTemplate
* Few changes to governance rpc
* Safety check in CInstantSend::SyncTransaction
* Full path in 'failed to load cache' warnings
* Refactor privateSendSigner
* Net Fixes/DNS Seed Fix
* Don't add non-current watchdogs to seen map
* [RPC] remove the option of having multiple timer interfaces
* Fix memory leak in httprpc.cpp
* Make KEY_SIZE a compile-time constant
* Fix memory leak in wallet tests
* Avoid OpenSSL certstore-related memory leak
* Avoid shutdownwindow-related memory leak
* Avoid splash-screen related memory leak
* Prevent thread/memory leak on exiting
* Fix various things -fsanitize complains about