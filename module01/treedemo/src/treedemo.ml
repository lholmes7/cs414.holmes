

let values = [10; 5; 15]

let aBintree = List.fold_left (fun t x -> Bintree.Tree.insert t x) Bintree.Tree.Empty values

let anAvlTree = List.fold_left (fun t x -> Avl.insert t x) Avl.Empty values
    
let tree_as_string = Bintree.Tree.string_of_tree aBintree
let () = print_endline tree_as_string
  
let avl_as_string = Avl.string_of_avl anAvlTree 
let () = print_endline avl_as_string
