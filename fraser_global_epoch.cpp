// global epoch
static atomic<unsigned int> m_nGlobalEpoch := 1 ;
const EPOCH_COUNT = 3 ;
// TLS data
struct ThreadEpoch {
    // global epoch of the thread
    unsigned int        m_nThreadEpoch ;
    // the list of retired elements
    List<void *>        m_arrRetired[ EPOCH_COUNT ] ;
   
    ThreadEpoch(): m_nThreadEpoch(1) {}
    void enter() {
       if ( m_nThreadEpoch <= m_nGlobalEpoch )
          m_nThreadEpoch = m_nGlobalEpoch + 1 ;
    }
    void exit() {
       if ( all threads are in the epoch which m_nGlobalEpoch ) {
          ++m_nGlobalEpoch ;
          empty (delete) the elements
          m_arrRetired[ (m_nGlobalEpoch – 2) % EPOCH_COUNT ]
          of all threads ;
       }
    }
} ;

lock_free_op( … ) {
    get_current_thread()->ThreadEpoch.enter() ;
    . . .
    // lock-free operation of the container.
    // we’re inside “the critical section” of the epoch-based scheme,
    // so we can be sure that no one will delete the data we’re working with.
    . . .
    get_current_thread()->ThreadEpoch.exit() ;
}