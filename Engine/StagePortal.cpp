# i n c l u d e   " p c h . h "  
 # i n c l u d e   " S t a g e P o r t a l . h "  
 # i n c l u d e   " T e x t O b j e c t . h "  
 # i n c l u d e   " U I O b j e c t . h "  
 # i n c l u d e   " S c e n e M a n a g e r . h "  
 # i n c l u d e   " S c e n e . h "  
 # i n c l u d e   " T i m e r . h "  
  
 v o i d   S t a g e P o r t a l : : U p d a t e ( )  
 {  
 	 i f   ( m _ i s A c t i v a t e d )  
 	 {  
 	 	 i f   ( m _ t i m e r   >   0 . f )  
 	 	 {  
 	 	 	 m _ t i m e r   - =   G E T _ S I N G L E ( T i m e r ) - > G e t D e l t a T i m e ( ) ;  
 	 	 }  
 	 }  
  
 }  
  
 v o i d   S t a g e P o r t a l : : P r i n t I n t e r a c t i v e T e x t ( )  
 {  
 	 w s t r i n g   t e x t { } ;  
 	  
 	 i f   ( m _ i s A c t i v a t e d )  
 	 {  
 	 	 i f   ( m _ t i m e r   >   0 . f )  
 	 	 	 t e x t   =   ( L " ��tǸ�  \�1�T�  �  . . . " ) ;  
 	 	 e l s e  
 	 	 	 t e x t   =   ( L " �L�  ��L�t���\�  t�ٳ" ) ;  
 	 }  
 	 e l s e  
 	 	 t e x t   =   ( L " ��tǸ�  \�1�T�" ) ;  
  
  
 	 G E T _ S I N G L E ( S c e n e M a n a g e r ) - > G e t A c t i v e S c e n e ( ) - > G e t I n t e r a c t i v e O b j e c t T e x t ( ) - > S e t T e x t ( t e x t ) ;  
 	 G E T _ S I N G L E ( S c e n e M a n a g e r ) - > G e t A c t i v e S c e n e ( ) - > G e t I n t e r a c t i v e O b j e c t T e x t ( ) - > S e t P o s i t i o n ( V e c 2 ( 0 . f ,   - 3 0 . f ) ) ;  
 	 G E T _ S I N G L E ( S c e n e M a n a g e r ) - > G e t A c t i v e S c e n e ( ) - > G e t I n t e r a c t i v e O b j e c t T e x t ( ) - > S e t V i s i b l e ( t r u e ) ;  
 }  
  
 v o i d   S t a g e P o r t a l : : I n t e r a c t i v e F u n c t i o n ( )  
 {  
         i f   ( m _ i s A c t i v a t e d   = =   f a l s e )  
         {  
                 i f   ( ! m _ i s P a c k e t P r o c e s s )   {  
                         s h a r e d _ p t r < P o r t a l O n P a c k e t >   p a c k e t   =   m a k e _ s h a r e d < P o r t a l O n P a c k e t > ( ) ;  
                         S E N D ( p a c k e t ) ;  
                 }  
  
                 m _ i s A c t i v a t e d   =   t r u e ;  
                 m _ t i m e r   =   6 0 . f ;  
                 m _ s p a w n I d x   =   0 ;  
                 m _ s p a w n T i m e r   =   0 . f ;  
  
                 v e c t o r < s h a r e d _ p t r < G a m e O b j e c t > > g a m e O b j e c t s   =   G E T _ S I N G L E ( S c e n e M a n a g e r ) - > G e t A c t i v e S c e n e ( ) - > G e t C o l l i d a b l e G a m e O b j e c t s ( ) ;  
  
                 f o r   ( a u t o &   g a m e O b j e c t   :   g a m e O b j e c t s )  
                 {  
                         i f   ( g a m e O b j e c t - > G e t M o n o b e h a v i o u r ( " E n e m y " ) )  
                         {  
                                 s h a r e d _ p t r < M o n o B e h a v i o u r >   s c r i p t E   =   g a m e O b j e c t - > G e t M o n o b e h a v i o u r ( " E n e m y " ) ;  
                                 s c r i p t E - > G e t R i g i d B o d y ( ) - > S e t S t a t i c ( t r u e ) ;  
                                 s c r i p t E - > G e t R i g i d B o d y ( ) - > M o v e T o ( V e c 3 ( - 1 0 0 0 0 0 0 . f ,   1 5 0 0 . f ,   0 . f ) ) ;  
                         }  
                 }  
         }  
         e l s e  
         {  
                 i f   ( m _ t i m e r   < =   0 )   {  
                         s h a r e d _ p t r < S t a g e C h a n g e P a c k e t >   p a c k e t   =   m a k e _ s h a r e d < S t a g e C h a n g e P a c k e t > ( ) ;  
                         S E N D ( p a c k e t ) ;  
                         G E T _ S I N G L E ( S c e n e M a n a g e r ) - > S t a r t N e x t S t a g e ( ) ;  
                 }  
         }  
 }  
 