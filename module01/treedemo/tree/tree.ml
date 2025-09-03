(* A binary tree type where each node contains an integer *)
type binary_tree =
  | Empty
  | Node of int * binary_tree * binary_tree

(* Creating a binary tree *)
let aTree =
  Node (1,
        Node (2, Empty, Empty),
        Node (3,
              Node (4, Empty, Empty),
              Empty))

(* Function to calculate the height of the tree *)
let rec height tree =
  match tree with
  | Empty -> 0
  | Node (_, left, right) ->
      1 + max (height left) (height right)

(* In-order traversal *)
let rec inorder tree =
  match tree with
  | Empty -> []
  | Node (value, left, right) ->
      inorder left @ [value] @ inorder right

(* Example of using the tree functions *)
let h = height aTree;; (* Should return 3 *)

let inorder_list = inorder aTree;; (* Should return [2; 1; 4; 3] *)

(* Insert a value into a binary search tree *)
let rec insert tree x =
  match tree with
  | Empty -> Node (x, Empty, Empty)
  | Node (value, left, right) ->
      if x < value then
        Node (value, insert left x, right)
      else
        Node (value, left, insert right x)

(* Search for a value in a binary search tree *)
let rec search tree x =
  match tree with
  | Empty -> false
  | Node (value, left, right) ->
      if x = value then true
      else if x < value then search left x
      else search right x

(* Helper function to find the minimum value in a binary search tree *)
let rec find_min tree =
  match tree with
  | Empty -> failwith "Tree is empty"
  | Node (value, Empty, _) -> value
  | Node (_, left, _) -> find_min left

(* Delete a node from a binary search tree *)
let rec delete tree x =
  match tree with
  | Empty -> Empty
  | Node (value, left, right) ->
      if x < value then
        Node (value, delete left x, right)
      else if x > value then
        Node (value, left, delete right x)
      else
        match left, right with
        | Empty, Empty -> Empty  (* No children *)
        | Empty, _ -> right      (* One child: right *)
        | _, Empty -> left       (* One child: left *)
        | _, _ ->                (* Two children *)
            let min_value = find_min right in
            Node (min_value, left, delete right min_value)

(* Pre-order traversal *)
let rec preorder tree =
  match tree with
  | Empty -> []
  | Node (value, left, right) ->
      [value] @ preorder left @ preorder right

(* Post-order traversal *)
let rec postorder tree =
  match tree with
  | Empty -> []
  | Node (value, left, right) ->
      postorder left @ postorder right @ [value]

(* Breadth-first search (BFS) traversal *)
let bfs tree =
  let rec aux queue acc =
    match queue with
    | [] -> List.rev acc
    | Empty :: tl -> aux tl acc
    | Node (value, left, right) :: tl ->
        aux (tl @ [left; right]) (value :: acc)
  in
  aux [tree] []

(* Function to calculate the balance factor of a node *)
let balance_factor tree =
  match tree with
  | Empty -> 0
  | Node (_, left, right) -> (height left) - (height right)

let rec string_of_tree t = 
  match t with
  | Empty -> "()"
  | Node (v, left, right) ->
      "Node(" ^ string_of_int v ^ ", "
      ^ string_of_tree left ^ ", "
      ^ string_of_tree right ^ ")"
